<h1>OS Assignment</h1>
<p>So Yeh, IM A GAMER</p>

<h3>Known Issues</h3>
* Helgrind will report a bug for the Process version in libpthread for sem_wait succeeding before a prior sem_post, I believe this is because I initialize it with a non-zero value. This does appear to be the only issue helgrind has with my process implementation.
* If a line is skipped request no is not incremented (makes sense), however the linecount is so it looks slightly off in the sim_out but it is correct. (Might fix if I get time.)
