
void count_ready_in(struct proc *p);

void count_ready_out(struct proc *p);

void count_ready2_in(struct proc *p); 

void count_ready2_out(struct proc *p); 

void count_running_in(struct proc *p);

void count_blocked_in(struct proc *p);

void count_blocked_out(struct proc *p);

void count_finished_in(struct proc *p);

void accounting(struct queue *q);

