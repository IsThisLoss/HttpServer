# HttpServer
<p>
  This is simple http-server which was written for the final exam of
  <a href="https://stepic.org/course/149">this</a> 
  stepic course 
</p>
<h4>Main idea</h4>
<p>
  There is main process, which starts one process to listen new incoming connections and predefined number of 
  worker-processes. When listen-process get new connection, it passes file descriptor to the least busy worker and further 
  only this process work with that connection. Also, as soon as any worker process died, the main process creates new one.  
</p>
