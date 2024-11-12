private import cpp

predicate is_pageview(Expr e) { e.getValue() = "user_stats" }

predicate is_current_time(Call c) {
  exists(Function f |
    c.getTarget() = f and
    f.getName() = "now" and
    f.getNamespace().getName() = "chrono"
  )
}
