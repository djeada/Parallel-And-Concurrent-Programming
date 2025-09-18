import threading
import time

def worker():
    for i in range(5):
        print(f"Worker running... {i}")
        time.sleep(1)

print("Main thread starting")

# Create a daemon thread
t = threading.Thread(target=worker, daemon=True)
t.start()

# --- Uncomment the next line to see the difference ---
# t.join()

print("Main thread finished")
