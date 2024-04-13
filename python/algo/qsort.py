def qsort(array):
    if len(array) < 2:
        return array
    else:
        pivot = array[0]
        less = [i for i in array[1:] if i < pivot]
        greater = [i for i in array[1:] if i >= pivot]
        return qsort(less) + [pivot] + qsort(greater)


if __name__ == "__main__":
    print(qsort([1, 5, -2, 10, 0, 6, 89, 100]))
