set follow-exec-mode new
set detach-on-fork off
set follow-fork-mode child
b msg_queue_push
