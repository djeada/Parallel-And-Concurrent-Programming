// A promise is actually not broken by never setting the value into the promise.
// This will just make the child wait forever inside f.get().
// The broken_promise exception gets thrown when the promise variable is
// destroyed by going out of scope.

//  one must always assign the return value of the call to
//  std::async(std::launch::async,...) to a variable (and not just call it
//  without assigning the return value),
// otherwise the call still blocks until execution of the thread finishes
// (because if not assigned, the return value can be destroyed immediately, and
// the destructor of that future waits for the task to finish).
