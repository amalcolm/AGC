# extra_scripts.py
Import("env")                                # type: ignore or # pylint: disable=undefined-variable
import socket

# --- Configuration ---
UDP_IP = "127.0.0.1"
UDP_PORT = 11000 # Make sure this matches your C# listener

def send_udp_message(message):
    """Sends a UDP message to the specified IP and port."""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(bytes(message, "ascii"), (UDP_IP, UDP_PORT))
        print(f"PlatformIO: Sent '{message}' command to plotter.")
    except Exception as e:
        print(f"PlatformIO: Could not send UDP message.  Error: {e}")

# --- Hook Functions ---
# This function will be called BEFORE the "upload" target is executed.
def before_upload(source, target, env):
    print("PlatformIO: Notifying plotter to release COM port...")
    send_udp_message("DISCONNECT")

# This function will be called AFTER the "upload" target is executed.
def after_upload(source, target, env):
    print("PlatformIO: Notifying plotter to reconnect...")
    send_udp_message("RECONNECT")

# --- Registering the Hooks ---
# The first argument is the target, the second is the function to call.
env.AddPreAction("upload", before_upload)    # type: ignore or # pylint: disable=undefined-variable
env.AddPostAction("upload", after_upload)   # type: ignore or # pylint: disable=undefined-variable