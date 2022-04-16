import multiprocessing
import time
import random


def create_matrix(size):
    matrix = [[random.randint(1, 1000) for i in range(size)] for j in range(size)]

    for i in range(size):
        for j in range(size):
            if i - 1 >= 0 and j - 1 >= 0 and i + 1 < size and j + 1 < size:
                matrix[i][j] = (
                    matrix[i][j]
                    * matrix[(i - 1) % size][j]
                    * matrix[(i + 1) % size][j]
                    * matrix[i][(j - 1) % size]
                    * matrix[i][(j + 1) % size]
                )

    return matrix


if __name__ == "__main__":

    inputs = list(range(15))
    start = time.time()
    outputs = list(map(create_matrix, inputs))
    end = time.time()
    print(f"Time taken: {end - start}")

    start = time.time()
    # use multiprocessing pool
    pool = multiprocessing.Pool(processes=4)
    outputs = pool.map(create_matrix, inputs)
    pool.close()
    pool.join()
    end = time.time()
    print(f"Time taken: {end - start}")
