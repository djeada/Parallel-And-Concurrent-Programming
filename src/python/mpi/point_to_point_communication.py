"""
Point-to-Point Communication

This example demonstrates basic point-to-point communication using
comm.send() and comm.recv(). These are blocking operations - the sender
waits until the message is safely buffered, and the receiver waits
until the message arrives.

Key concepts:
- comm.send(data, dest, tag): Blocking send operation
- comm.recv(source, tag): Blocking receive operation
- Message envelope: (source, destination, tag, communicator)
- Python objects are automatically serialized (pickled)

Note: mpi4py provides lowercase methods (send/recv) for Python objects
and uppercase methods (Send/Recv) for numpy arrays with better performance.

Run: mpirun -np 2 python point_to_point_communication.py
"""

from mpi4py import MPI


def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    if size < 2:
        if rank == 0:
            print("Error: This program requires at least 2 processes.")
        return

    TAG = 0

    if rank == 0:
        # Process 0 sends data to process 1
        data = {"value": 42, "message": "Hello from process 0"}
        print(f"Process 0: Sending {data} to process 1")

        comm.send(data, dest=1, tag=TAG)
        print("Process 0: Message sent successfully")

    elif rank == 1:
        # Process 1 receives data from process 0
        data = comm.recv(source=0, tag=TAG)
        print(f"Process 1: Received {data} from process 0")

    # Demonstrate sending different data types
    if rank == 0:
        # Send various Python objects
        comm.send([1, 2, 3, 4, 5], dest=1, tag=1)  # List
        comm.send("Hello MPI!", dest=1, tag=2)     # String
        comm.send(3.14159, dest=1, tag=3)          # Float
        print("Process 0: Sent list, string, and float to process 1")

    elif rank == 1:
        # Receive the data
        list_data = comm.recv(source=0, tag=1)
        string_data = comm.recv(source=0, tag=2)
        float_data = comm.recv(source=0, tag=3)

        print(f"Process 1: Received list: {list_data}")
        print(f"Process 1: Received string: {string_data}")
        print(f"Process 1: Received float: {float_data}")


if __name__ == "__main__":
    main()
