"""
Cartesian Topology

This example demonstrates how to create a Cartesian topology using
MPI.Compute_dims() and comm.Create_cart(). Cartesian topologies are
useful for problems with regular grid structures.

Key concepts:
- MPI.Compute_dims(): Helper to determine optimal grid dimensions
- comm.Create_cart(): Creates a Cartesian communicator
- cart_comm.Get_coords(): Gets coordinates of a process in the grid
- cart_comm.Shift(): Finds neighbor ranks in a given direction
- Periodic boundaries: Can wrap around edges (like a torus)

Use cases:
- Finite difference/element methods
- Image processing
- Any algorithm with regular grid data decomposition

Run: mpirun -np 6 python cartesian_topology.py
"""

from mpi4py import MPI


def main():
    comm = MPI.COMM_WORLD
    world_rank = comm.Get_rank()
    world_size = comm.Get_size()

    # Create a 2D Cartesian grid
    ndims = 2

    # Let MPI determine optimal dimensions
    dims = MPI.Compute_dims(world_size, [0, 0])

    if world_rank == 0:
        print(f"Creating {dims[0]}x{dims[1]} Cartesian grid with {world_size} processes")
        print()

    # Periodic boundaries: True = periodic (wraps around), False = non-periodic
    periods = [True, True]  # Periodic in both dimensions (torus)

    # reorder = True allows MPI to reorder ranks for efficiency
    reorder = True

    # Create the Cartesian communicator
    cart_comm = comm.Create_cart(dims, periods=periods, reorder=reorder)

    # Get rank and coordinates in the new communicator
    cart_rank = cart_comm.Get_rank()
    coords = cart_comm.Get_coords(cart_rank)

    print(f"World rank {world_rank} -> Cart rank {cart_rank} at coords {coords}")

    # Find neighbors using Shift
    # Direction 0 = row (up/down), Direction 1 = column (left/right)
    up, down = cart_comm.Shift(0, 1)      # Shift in row direction
    left, right = cart_comm.Shift(1, 1)   # Shift in column direction

    print(f"Cart rank {cart_rank} neighbors: up={up}, down={down}, left={left}, right={right}")

    # Demonstrate communication with neighbors
    cart_comm.Barrier()

    if cart_rank == 0:
        print()
        print("=== Neighbor Communication ===")

    cart_comm.Barrier()

    # Send rank to right neighbor, receive from left neighbor
    my_value = cart_rank * 10
    received = cart_comm.sendrecv(my_value, dest=right, source=left)
    print(f"Cart rank {cart_rank}: Sent {my_value} right, received {received} from left")

    # Clean up
    cart_comm.Free()


if __name__ == "__main__":
    main()
