def fun7(n1, target):
    if n1 is None:
        return -1
    if n1.value > target:  # Compare the value of the node
        return 2 * fun7(n1.left, target)
    elif n1.value < target:  # Compare the value of the node
        return 2 * fun7(n1.right, target) + 1
    else:
        return 0

def secret_phase(input_number):
    if input_number < 1 or input_number > 1000:
        print("Bomb exploded!")
        return

    n1 = create_tree()  # Assume this function creates the tree structure used in fun7
    result = fun7(n1, input_number)

    if result == 3:
        print("Phase defused!")
    # else:
    #     print("Bomb exploded!")

def create_tree():
    # This function should create the tree structure used in fun7
    # For demonstration purposes, we'll return a mock object
    class Node:
        def __init__(self, value, left=None, right=None):
            self.value = value
            self.left = left
            self.right = right

    # Example tree structure
    return Node(500, Node(250), Node(750))

# Example usage
input_number = [n for n in range(1,1001)]
for n in input_number:
    secret_phase(n)