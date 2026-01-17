"""
Communicators and Groups

This example demonstrates how to create sub-communicators using
comm.Split(). This is useful for organizing processes into groups
for modular parallel algorithms.

Key concepts:
- comm.Split(color, key): Splits communicator into sub-communicators
- Color: Determines which sub-communicator a process joins
- Key: Determines the rank ordering within the new communicator
- Processes with the same color join the same sub-communicator
- MPI.UNDEFINED color excludes a process from any sub-communicator

Use cases:
- Hierarchical parallelism (e.g., nodes vs cores)
- Isolating communication in different parts of the program
- Creating library-safe communication contexts

Run: mpirun -np 8 python communicators_and_groups.py
"""

from mpi4py import MPI


def main():
    comm = MPI.COMM_WORLD
    world_rank = comm.Get_rank()
    world_size = comm.Get_size()

    # Split processes into two groups based on rank parity
    # Even ranks (0, 2, 4, ...) get color 0
    # Odd ranks (1, 3, 5, ...) get color 1
    color = world_rank % 2

    # Use world_rank as key to preserve relative ordering
    new_comm = comm.Split(color, world_rank)

    # Get rank within the new communicator
    new_rank = new_comm.Get_rank()
    new_size = new_comm.Get_size()

    group_name = "EVEN" if color == 0 else "ODD"
    print(f"World rank {world_rank} -> Group {group_name}, "
          f"new rank {new_rank} of {new_size}")

    # Demonstrate communication within the sub-communicator
    # Reduce within each group separately
    group_sum = new_comm.reduce(world_rank, op=MPI.SUM, root=0)

    if new_rank == 0:
        print(f"Group {group_name} sum of world ranks: {group_sum}")

    # Demonstrate broadcast within sub-communicator
    if new_rank == 0:
        group_message = f"Hello from {group_name} group leader"
    else:
        group_message = None

    group_message = new_comm.bcast(group_message, root=0)
    print(f"World rank {world_rank}: Received in-group message: '{group_message}'")

    # Clean up the new communicator
    new_comm.Free()


if __name__ == "__main__":
    main()
