#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

// struct queue {
//   struct proc* arr[NPROC];
//   int len;
//   int head;
// } q;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

// void
// printq(void)
// {
//   cprintf("ENQUEUE: ");
//   for (int i = 0; i < q.len; i++)
//     cprintf("%d ", q.arr[(q.head + i)%NPROC]->pid);
//   cprintf("\n");
// }

// void
// _enqueue(struct proc* p, struct queue* q)
// {
//   q->arr[(q->head + q->len) % NPROC] = p;
//   q->len++;
//   //printq();
// }

// void
// enqueue(struct proc* p)
// {
//   if (p->priority < 5)
//     _enqueue(p, q1);
//   else if (p->priority < 10)
//     _enqueue(p, q2);
//   else if (p->priority < 15)
//     _enqueue(p, q3);
//   else
//     _enqueue(p, q4);
// }


// struct proc*
// dequeue()
// {
//   if (!q.len)
//     return 0;

//   struct proc* p = q.arr[q.head];
//   q.head = (q.head + 1) % NPROC;
//   q.len--;

//   if (p->state != RUNNABLE)
//     return 0;

//   cprintf("DEQUEUE: [%d]\n", p->pid);
//   return p;
// }

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;

  p->tick = 0;
  p->priority = 5; // Default priority
  cprintf("Allocproc\n");

  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  p->signal = 0;                  // initizlize signal
  memset(p->sighandlers, 0, 32);  // initialize signal handlers to 0 (NULL)

  return p;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;
  p->in_time = ticks;

  // set schedule type
  SCHED_TYPE = SCHED_RR;

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = cowuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;
  memcpy(np->sighandlers, curproc->sighandlers, 32);

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;
  np->in_time = ticks; // time that state is changed into RUNNABLE

  // change sh proc's priority to 3
  if(np->name[0] == 's' && np->name[1] == 'h')
    np->priority = 3;

  release(&ptable.lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  if(curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  //cprintf("in exit, process name: %s, pid: %d parentpid: %d\n", curproc->name, curproc->pid, curproc->parent->pid);
  // Send SIGCHILDEXIT signal to its parent process
  sigsend(curproc->parent->pid, SIGCHILDEXIT);
  
  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p = ptable.proc;
  struct proc *p2, *sched_proc;  // for scheduling
  struct cpu *c = mycpu();
  int mint = 0; // for FIFO scheduling
  int flag;     // for MLQ scheduling
  c->proc = 0;
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);

    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // process scheduling
      switch(SCHED_TYPE) {          
        case SCHED_RR:
          break;

        case SCHED_FIFO:
          mint = 0xffffff; // max uint value
          sched_proc = ptable.proc;
          for(p2 = ptable.proc; p2 < &ptable.proc[NPROC]; p2++){
            if(p2->state != RUNNABLE)
              continue;
            if (p2->in_time < mint)
              sched_proc = p2;
              mint = p2->in_time;
          }
          p = sched_proc;
          break;

        case SCHED_PRIORITY:
          sched_proc = p;

          // choose a proc with highest priority
          for(p2 = ptable.proc; p2 < &ptable.proc[NPROC]; p2++){
            if(p2->state != RUNNABLE)
              continue;
            if (sched_proc->priority > p2->priority)
              sched_proc = p2;
          }
          p = sched_proc;
          break;

        // Level1,2,3 queue is FIFO scheduling
        // Level4 queue is RR scheduling
        case SCHED_MLQ:
          flag = 0;
          for (p2 = ptable.proc; p2 < &ptable.proc[NPROC]; p2++) { // 1Level priority process
            if (p2->state == RUNNABLE && p2->priority < 5){
              p2->priority += 1;
              p2->mlq_level = 1;
              p = p2;
              flag = 1;
              break;
            }
          }
          if (flag) break;

          for (p2 = ptable.proc; p2 < &ptable.proc[NPROC]; p2++) { // 2Level priority process
            if (p2->state == RUNNABLE && p2->priority >= 5 && p2->priority < 10) {
              p2->priority += 1;
              p2->mlq_level = 2;
              p = p2;
              flag = 1;
              break;
            }
          }
          if (flag) break;

          for (p2 = ptable.proc; p2 < &ptable.proc[NPROC]; p2++) { // 3Level priority process
            if (p2->state == RUNNABLE && p2->priority >= 10 && p2->priority < 15) {
              p2->priority += 1;
              p2->mlq_level = 3;
              p = p2;
              flag = 1;
              break;
            }
          }
          if (flag) break;

          for (p2 = p; p2 < &ptable.proc[NPROC]; p2++) { // 4Level priority process
            if (p2->state == RUNNABLE) {
              p2->mlq_level = 4;
              p = p2;
              break;
            }
          }
          break;

        default:  // default is RR
          break;
      }

      //if (p->state != RUNNABLE) continue;


      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);

      // Signal frmaework. Register handlers.
      if (p->signal != 0) {
        uint mask = (1 << 31);
        sighandler_t* handler = &p->sighandlers[31];

        while (mask > 4) {
          if ((p->signal & mask) && (*handler != 0))
            register_handler(*handler);
          
          mask >>= 1;   // Move the mask to the next bit to check
          handler--;    // Move the pointer to the next handler
        }

        // Default handlers (number 0~2) if there is no given handler
        while (mask > 0) {
          if (p->signal & mask) {
            if (*handler == 0) {
              switch(mask) {
                case 4: sigchildexit(); break;
                case 2: sigkillchild(); break;
                case 1: sigint(); break;
                default: break;
              }
            } else register_handler(*handler); 
          }

          mask >>= 1;
          handler--; 
        }

        p->signal = 0;  // reset signal
      }

      p->state = RUNNING;
      //if(ticks%100 == 0) 
      // cprintf("DEBUG: RUNNING %s [%d] %d\n", p->name, p->pid, p->priority);

      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
    release(&ptable.lock);

  }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  myproc()->state = RUNNABLE;
  myproc()->in_time = ticks;
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan) {
      p->state = RUNNABLE;
      p->in_time = ticks;
    }
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING) {
        p->state = RUNNABLE;
        p->in_time = ticks;
      }
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

// Regist the handler
void
register_handler(sighandler_t sighandler)
{ 
  struct proc *curproc = myproc();
  char* addr = uva2ka(curproc->pgdir, (char*)curproc->tf->esp);
  if ((curproc->tf->esp & 0xFFF) == 0)
    panic("esp_offset == 0");

  // Open a new frame
  *(int*)(addr + ((curproc->tf->esp - 4) & 0xFFF)) = curproc->tf->eip;
  curproc->tf->esp -= 4;
  // update eip
  curproc->tf->eip = (uint)sighandler;
}

void sigint() {
  //cprintf("IN SIGINT %d\n" , myproc()->pid);
  myproc()->killed = 1;
}

void sigkillchild() {
  struct proc *p, *curproc = myproc();
  cprintf("IN SIGKILLCHILD %d\n",curproc->pid);

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if(p->state == UNUSED)
      continue;

    if (p->parent->pid == curproc->pid) {
      cprintf("I'M A CHILD %d", p->pid);
      sigsend(p->pid, SIGINT);
    }
  }
}

void sigchildexit() {
  //cprintf("SIGCHILDEXIT (one of my child terminated) %d\n", myproc()->pid);
}

void killcurproc(void) {
  struct proc *p;
  cprintf("IN KILLCURPROC\n");

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if(p->state == UNUSED)
      continue;
      
    if ((p != initproc) && (p->pid != 2)) { //FIXME: state
      cprintf("IN KILLCURPROC LOOP %d\n",p->pid);
      sigsend(p->pid, SIGKILLCHILD);
      sigsend(p->pid, SIGINT);
      break;
    }
  }
}

int
signal(int signum, sighandler_t handler)
{
  struct proc *curproc = myproc();

  curproc->sighandlers[signum] = handler;
  return 0;
}

// Send signal (signum) to the process with the given pid.
int
sigsend(int pid, int signum)
{
  struct proc *p;
  
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->signal |= (1 << signum);
      return 0;
    }
  }

  return -1;
}

// Current process status
int
cps(void)
{
  struct proc *p;

  // Enable interrupts on this processor.
  sti();

  // Loop over process table looking for process to run.
  acquire(&ptable.lock);
  cprintf("name |\t pid |\t state |\t priority \n");
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if (p->state == RUNNING)
      cprintf("%s \t %d \t RUNNING \t %d \t \n", p->name, p->pid, p->priority);
    else if (p->state == RUNNABLE)
      cprintf("%s \t %d \t RUNNABLE \t %d \t \n", p->name, p->pid, p->priority);
    else if (p->state == SLEEPING)
      cprintf("%s \t %d \t SLEEPING \t %d \t \n", p->name, p->pid, p->priority);
  }

  release(&ptable.lock);

  return 24;
}

// Change priority
int
chpr(int pid, int priority)
{
  struct proc *p;

  acquire(&ptable.lock);
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if (p->pid == pid) {
      p->priority = priority;
      break;
    }
  }
  release(&ptable.lock);

  return pid;
}