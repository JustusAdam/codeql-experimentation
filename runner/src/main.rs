use std::{
    collections::HashMap,
    fs::File,
    path::{Path, PathBuf},
    process::Command,
    time::Duration,
};

use anyhow::Context;
use clap::Parser;
use indicatif::ProgressBar;

#[derive(Parser)]
pub struct Args {
    /// Path to the run configuration
    pub path: PathBuf,
    /// Populate the expected results files
    #[clap(long)]
    pub create_expected: bool,
    /// The command to run codeql
    #[clap(long, default_value = "codeql")]
    pub codeql_command: PathBuf,
    /// The name of the databases to create
    #[clap(long, default_value = "qdb")]
    pub database_name: String,
    /// If provided the codeql outputs will be stored here.
    #[clap(long)]
    pub keep_intermediates: bool,
    #[clap(long, default_value = "results")]
    pub results_dir: PathBuf,
}

#[derive(serde::Deserialize)]
pub struct Config {
    pub application: HashMap<String, Application>,
    pub experiment: Box<[Experiment]>,
}

impl Config {
    pub fn iter_runs<'a>(&'a self) -> impl Iterator<Item = Run<'a>> + 'a {
        self.experiment
            .iter()
            .enumerate()
            .map(move |(id, experiment)| {
                let application = self
                    .application
                    .get(&experiment.application)
                    .expect("Application not found");
                Run {
                    id: id as u32,
                    application,
                    experiment,
                }
            })
    }
}

pub struct Run<'a> {
    pub id: u32,
    pub application: &'a Application,
    pub experiment: &'a Experiment,
}

impl Run<'_> {
    pub fn steps(&self) -> u64 {
        2 + self.application.libs.len() as u64
    }

    fn cmake_build(&self, executor: &mut Executor, base_dir: &Path) {
        let build_dir = base_dir.join(&self.application.build_dir);
        std::fs::create_dir_all(&build_dir).unwrap();
        let mut cmd = Command::new("cmake");
        cmd.arg("..").current_dir(&build_dir);
        executor.run_command(&mut cmd, true);
        let mut cmd = Command::new("cmake");
        cmd.arg("--build").arg(".").current_dir(&build_dir);
        executor.run_command(&mut cmd, true);
    }

    fn build_lib(&self, executor: &mut Executor, lib: &PathBuf) {
        self.cmake_build(executor, &self.application.path.join(lib));
    }

    fn build_application(&self, executor: &mut Executor) {
        let codeql_build_dir = self.application.path.join("_codeql_build_dir");
        if codeql_build_dir.exists() {
            std::fs::remove_dir_all(&codeql_build_dir).unwrap();
        }

        let mut cmd = Command::new(&executor.codeql_command);
        cmd.current_dir(&self.application.path)
            .args(["database", "create", "--overwrite", "--language=cpp"])
            .arg(&executor.args.database_name);
        executor.run_command(&mut cmd, true);
    }

    fn run_codeql(&self, executor: &mut Executor) {
        let args = &executor.args;
        let db_path = self.application.path.join(&args.database_name);

        let codeql_out = if args.create_expected {
            std::fs::create_dir_all(self.experiment.expected.parent().unwrap()).unwrap();
            self.experiment.expected.clone()
        } else {
            executor
                .intermediates
                .join(format!("{}-{}", self.id, self.experiment.application))
        };

        let mut cmd = Command::new(&executor.codeql_command);
        cmd.args(["query", "run", "-d"])
            .arg(db_path)
            .arg(&self.experiment.policy)
            .stdout(File::create(&codeql_out).unwrap());

        executor.run_command(&mut cmd, false);

        if !executor.args.create_expected {
            let expected = std::fs::read_to_string(&self.experiment.expected).unwrap();
            let actual = std::fs::read_to_string(&codeql_out).unwrap();
            assert_eq!(expected, actual);
            if !executor.args.keep_intermediates {
                std::fs::remove_file(&codeql_out).unwrap();
            }
        }
    }

    pub fn execute(&self, executor: &mut Executor) {
        let progress = executor.progress.clone();
        for lib in self.application.libs.iter() {
            progress.set_message(format!(
                "lib({}:{})",
                self.experiment.application,
                lib.display()
            ));
            self.build_lib(executor, lib);
            progress.inc(1);
        }
        progress.set_message(format!("db({})", self.experiment.application));
        self.build_application(executor);
        progress.inc(1);
        progress.set_message(format!(
            "policy({}:{})",
            self.experiment.application,
            self.experiment
                .policy
                .file_name()
                .unwrap()
                .to_str()
                .unwrap()
        ));
        self.run_codeql(executor);
        progress.inc(1);
    }
}

#[derive(serde::Deserialize)]
pub struct Experiment {
    pub application: String,
    pub policy: PathBuf,
    pub expected: PathBuf,
}

#[derive(serde::Deserialize)]
pub struct Application {
    #[serde(default)]
    pub libs: Box<[PathBuf]>,
    #[serde(default = "const_build")]
    pub build_dir: PathBuf,
    pub path: PathBuf,
}

fn const_build() -> PathBuf {
    PathBuf::from("build")
}

pub struct Executor {
    pub args: &'static Args,
    pub config: &'static Config,
    pub progress: ProgressBar,
    pub stdout: File,
    pub stderr: File,
    pub results_dir: PathBuf,
    pub intermediates: PathBuf,
    pub codeql_command: PathBuf,
}

impl Executor {
    pub fn new(args: Args) -> Self {
        let config: Config =
            toml::de::from_str(std::fs::read_to_string(&args.path).unwrap().as_str())
                .with_context(|| format!("Getting config from {}", &args.path.display()))
                .unwrap();
        let progress = ProgressBar::new(0).with_style(
            indicatif::ProgressStyle::with_template(
                "[{msg:15}] {wide_bar} {pos:>4}/{len:4} {elapsed:7}",
            )
            .unwrap(),
        );
        let bench_num = chrono::Utc::now().format("%Y-%m-%dT%H:%M:%S");
        progress.enable_steady_tick(Duration::from_millis(500));

        let results_dir = args.results_dir.join(format!("{bench_num}"));
        std::fs::create_dir_all(&results_dir).unwrap();
        let stdout = File::create(results_dir.join("stdout.log")).unwrap();
        let stderr = File::create(results_dir.join("stderr.log")).unwrap();
        let intermediates = results_dir.join("tmp");
        std::fs::create_dir_all(&intermediates).unwrap();
        Self {
            codeql_command: args
                .codeql_command
                .canonicalize()
                .with_context(|| format!("Canonicalizing {}", args.codeql_command.display()))
                .unwrap(),
            args: Box::leak(Box::new(args)),
            config: Box::leak(Box::new(config)),
            progress,
            stdout,
            stderr,
            results_dir,
            intermediates,
        }
    }

    pub fn run_command(&mut self, command: &mut Command, set_stdout: bool) {
        use std::io::Write;
        command.stderr(self.stderr.try_clone().unwrap());
        if set_stdout {
            command.stdout(self.stdout.try_clone().unwrap());
        }
        write!(self.stdout, "Running: {:?}\n", command).unwrap();
        write!(self.stderr, "Running: {:?}\n", command).unwrap();
        let status = command.status().unwrap();
        assert!(status.success());
    }
}

fn main() {
    let args = Args::parse();
    let mut executor = Executor::new(args);

    let runs = executor.config.iter_runs().collect::<Vec<_>>();
    executor
        .progress
        .set_length(runs.iter().map(Run::steps).sum());

    for run in runs {
        run.execute(&mut executor);
    }
}
