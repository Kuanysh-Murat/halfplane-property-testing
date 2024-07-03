from utility import *
import sys


def main():
    file_name = input("File name: ")
    with open("./All images/" + file_name, "r") as file:
        original_stdin = sys.stdin
        sys.stdin = file
        n = int(input())
        matrix = [None for _ in range(n)]
        for i in range(n):
            line = input()
            matrix[i] = line
        image = matrix_to_image(matrix)
        image.show()
        sys.stdin = original_stdin
        if input("Save (y/n)?: ") == "y":
            image.save(f"Images png/{file_name}.png")


if __name__ == "__main__":
    main()
