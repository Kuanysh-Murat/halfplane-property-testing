from PIL import Image, ImageDraw
from typing import List
import random
from time import sleep
import subprocess

random.seed()

black_pixel = "#"
white_pixel = "."


def matrix_to_string(matrix: List[List[str]]) -> str:
    return "".join([cell for row in matrix for cell in row])


def matrix_to_image(matrix, pixel_size=1) -> Image.Image:
    n = len(matrix)
    image = Image.new("L", (n * pixel_size, n * pixel_size))
    draw = ImageDraw.Draw(image)
    for y in range(n):
        for x in range(n):
            color = 0 if matrix[y][x] == black_pixel else 240
            draw.rectangle(
                [
                    (x * pixel_size, y * pixel_size),
                    ((x + 1) * pixel_size - 1, (y + 1) * pixel_size - 1),
                ],
                fill=color,
            )
    return image


def create_random(n):
    return create_p_image(n, 0.5)


def get_random_line(n):
    x1 = random.uniform(-1, n)
    y1 = random.uniform(-1, n)
    x2 = random.uniform(-1, n)
    y2 = random.uniform(-1, n)
    d = x1 * y2 - x2 * y1
    if d == 0:
        return (y1, -x1, 0)
    else:
        return ((y1 - y2) / d, (x2 - x1) / d, 1)


def create_halfplane(n):
    ...
    # return create_function_image(n, lambda x: slope * x + b)


def create_epsilon_close(n, epsilon, max_possible=True):
    max_count = int(n * n * epsilon)
    change_count = max_count if max_possible else random.randint(1, max_count)
    matrix = create_halfplane(n)
    for _ in range(change_count):
        y = random.randint(0, n - 1)
        x = random.randint(0, n - 1)
        matrix[y][x] = not matrix[y][x]
    return matrix


def create_p_image(n, p):
    return [
        [(True if random.random() <= p else False) for _ in range(n)] for _ in range(n)
    ]


def create_function_image(n, f):
    matrix = [[None for _ in range(n)] for _ in range(n)]
    for y in range(n):
        for x in range(n):
            color = True if y > f(x) else False
            matrix[y][x] = color
    return matrix


def find_line_distance(matrix, p1, p2, min_distance):
    n = len(matrix)
    vertical = p2[0] == p1[0]
    slope = 0
    b = 0
    if not vertical:
        slope = (p2[1] - p1[1]) / (p2[0] - p1[0])
        b = p1[1] - slope * p1[0]
    black_more = 0
    white_more = 0
    black_less = 0
    white_less = 0
    for y in range(n):
        for x in range(n):
            if (vertical and x <= p1[0]) or y >= slope * x + b:
                if matrix[y][x]:
                    black_more += 1
                else:
                    white_more += 1
            else:
                if matrix[y][x]:
                    black_less += 1
                else:
                    white_less += 1
            if min(white_more + black_less, black_more + white_less) >= min_distance:
                return min_distance
    return min(white_more + black_less, black_more + white_less)


def find_epsilon_distance(matrix):
    n = len(matrix)
    black_count = sum(
        [1 if cell == black_pixel else 0 for row in matrix for cell in row]
    )
    min_distance = min(black_count, n * n - black_count)
    for i in range(n * n):
        for j in range(i):
            p1 = (i % n, i // n)
            p2 = (j % n, j // n)
            min_distance = min(
                min_distance, find_line_distance(matrix, p1, p2, min_distance)
            )
    return min_distance / (n * n)


def find_point_line_distance(p1, p2, point):
    vertical = p2[0] == p1[0]
    m0 = 0
    b0 = 0
    if not vertical:
        m0 = (p2[1] - p1[1]) / (p2[0] - p1[0])
        b0 = p1[1] - m0 * p1[0]
    x0, y0 = point
    m1 = -1 / m0
    b1 = y0 - m1 * x0
    x1 = (b1 - b0) / (m0 - m1)
    y1 = m0 * x1 + b0
    return ((x1 - x0) ** 2 + (y1 - y0) ** 2) ** 0.5


def create_gradient(n, pdf):
    matrix = [[None for _ in range(n)] for _ in range(n)]
    x1 = random.uniform(-1, n)
    y1 = random.uniform(-1, n)
    x2 = random.uniform(-1, n)
    y2 = random.uniform(-1, n)
    slope = (y2 - y1) / (x2 - x1)
    b = y1 - slope * x1
    for y in range(n):
        for x in range(n):
            dist = find_point_line_distance((x1, y1), (x2, y2), (x, y))
            if y < slope * x + b:
                dist = -dist
            matrix[y][x] = random.random() <= pdf(dist)
    return matrix


# def get_images(image_size):
#     file_names = os.listdir(images_path)
#     selected_images = []
#     for name in file_names:
#         with open(images_path + name, "r") as file:
#             if len(file.readlines()) == image_size:
#                 selected_images.append(name)
#     return sorted(selected_images)


# def show(epsilon_distance, image_size):
#     print(f"{image_size}x{image_size} image will be shown")
#     labels = get_images(image_size)

#     dist_str = format(epsilon_distance, ".9f")
#     dist_str = labels[bisect.bisect_left(labels, dist_str)] if labels else ""
#     if not dist_str:
#         print("No such image exists")
#         return
#     # file_name = root + dist_str
#     lines = []
#     try:
#         with open(file_name, "r") as file:
#             lines = file.readlines()
#     except:
#         pass
#     image_size = len(lines)
#     matrix = [
#         [(True if line[i] == "#" else False) for i in range(image_size)]
#         for line in lines
#     ]
#     image = matrix_to_image(matrix, 1000 // image_size)
#     image.show()
#     print(f"Epsilon distance = {dist_str}")


def test(): ...


id_process_table = {}


def is_running(pid):
    return id_process_table[pid].poll() is None


def start_process(image_size):
    process = subprocess.Popen(
        [
            "./generator",
            f"{image_size}",
        ]
    )
    pid = process.pid
    print(f"Size = {image_size}, pid = {pid}")
    id_process_table[pid] = process
    return pid


def generateIndefinitely():
    start = 900
    end = 1500
    step = 100
    max_process_count = 12

    image_size = start
    process_ids = []
    while True:
        new_process_ids = []
        for pid in process_ids:
            if is_running(pid):
                new_process_ids.append(pid)
        process_ids = new_process_ids
        while len(process_ids) < max_process_count:
            pid = start_process(image_size)
            process_ids.append(pid)
            image_size += step
            if image_size > end:
                image_size = start
        sleep(1)


def reservoir_sampling(stream, k):
    reservoir = []
    for i, item in enumerate(stream):
        if i < k:
            reservoir.append(item)
        else:
            j = random.randint(0, i)
            if j < k:
                reservoir[j] = item
    reservoir = set(reservoir)
    sample = []
    for elem in stream:
        if elem in reservoir:
            sample.append(elem)
    return sample
