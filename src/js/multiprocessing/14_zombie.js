import multiprocessing


def spawn_numbers(pipe):
    input_pipe, _ = pipe
    for i in range(10):
        input_pipe.send(i)
    input_pipe.close()


if __name__ == "__main__":

    pipe = multiprocessing.Pipe(True)
    process = multiprocessing.Process(target=spawn_numbers, args=(pipe,))
    process.start()
    process.join()

    _, output_pipe = pipe

    try:
        while 1:
            print(output_pipe.recv())
    except EOFError:
        print("End")
