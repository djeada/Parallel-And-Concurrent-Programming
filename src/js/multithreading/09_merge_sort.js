import concurrent.futures


def merge_sort_thread(arr):
    def merge(arr_a, arr_b):
        result = list()
        index_a = index_b = 0

        while len(arr_a) + len(arr_b) > index_a + index_b:

            if len(arr_b) <= index_b or (
                len(arr_a) > index_a and arr_a[index_a] < arr_b[index_b]
            ):
                result.append(arr_a[index_a])
                index_a += 1

            else:
                result.append(arr_b[index_b])
                index_b += 1

        return result

    n = len(arr)

    if n < 2:
        return arr

    # each instance should be in a separate thread
    with concurrent.futures.ThreadPoolExecutor() as executor:
        future_a = executor.submit(merge_sort_thread, arr[: n // 2])
        arr_a = future_a.result()
        future_b = executor.submit(merge_sort_thread, arr[n // 2 :])
        arr_b = future_b.result()

    return merge(arr_a, arr_b)


array = [4, 2, 1, 5, 3, 6, 7, 8, 9, 0]

print(merge_sort_thread(array))
