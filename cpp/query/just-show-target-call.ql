import cpp

from Call c
where c.getTarget().getName() = "target"
select c, c.getLocation().getStartLine()
