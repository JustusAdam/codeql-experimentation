use anyhow::Result;
use std::sync::Arc;

use paralegal_policy::{
    assert_error,
    paralegal_spdg::{traverse::EdgeSelection, Identifier},
    Context, Diagnostics, Marker,
};

macro_rules! marker {
    ($id:ident) => {
        Marker::new_intern(stringify!($id))
    };
}

pub fn check(ctx: Arc<Context>) -> Result<()> {
    ctx.named_policy(Identifier::new_intern("check"), |ctx| {
        let is_compliant = ctx.all_controllers().all(|(c_id, _ctrl)| {
            let mut source_nodes = ctx
                .all_nodes_for_ctrl(c_id)
                .filter(|n| ctx.has_marker(marker!(source), *n));

            source_nodes.all(|source| {
                let mut sink_nodes = ctx
                    .all_nodes_for_ctrl(c_id)
                    .filter(|n| ctx.has_marker(marker!(sink), *n));

                sink_nodes.all(|sink| {
                    !ctx.influencers(sink, EdgeSelection::Data)
                        .any(|n| n == source)
                })
            })
        });
        assert_error!(
            ctx,
            is_compliant,
            format!("At least one controller does not satisfy the policy")
        );
        Ok(())
    })
}

use std::path::PathBuf;

use clap::Parser;

/// Cleans the environment if we are being run as "cargo run"
///
/// Also disables incremental computation to reduce the size of compile
/// artifacts generated during analysis.
fn env_setup() {
    use std::env;
    for (k, _) in env::vars() {
        if k.starts_with("CARGO") || k.starts_with("RUSTUP") {
            env::remove_var(k)
        }
    }
    env::set_var("CARGO_INCREMENTAL", "false");
}

/// Runnable Paralegal policy generated from simple-source-sink.txt.
///
/// This program has a set of default command line arguments that
/// tend to be useful.
///
/// By default running this program will first invoke the PDG generator
/// (`cargo paralegal-flow`) automatically in the target directory
/// (`dir` argument) and then enforce the policy on the output graph.
/// You may pass `--skip-pdg-gen` to skip this step and only run the
/// policy.
#[derive(clap::Parser)]
struct Args {
    /// Directory of the program that should be analyzed
    dir: PathBuf,
    /// If you want to include an external annotation file during PDG generation
    /// provide it here.
    #[clap(long, short)]
    external_annotations: Option<PathBuf>,
    /// Do not run the PDG generation, only enforce the policy. Assumes that `dir`
    /// contains a `flow-graph.o` output file from a prior PDG generation run.
    #[clap(long)]
    skip_pdg_gen: bool,
    /// Additional arguments that should be passed to the PDG generator.
    #[clap(last = true)]
    flow_args: Vec<String>,
}

fn main() -> Result<()> {
    let args = Args::parse();
    env_setup();
    let mut cmd = paralegal_policy::SPDGGenCommand::global();
    if let Some(p) = args.external_annotations.as_ref() {
        cmd.external_annotations(p);
    }
    cmd.get_command().args(args.flow_args.iter());
    cmd.run(&args.dir)?.with_context(check)?;
    println!("Policy successful");
    Ok(())
}
