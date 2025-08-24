import socket

HOST = "192.168.1.158"  # IP of Arduino Board
PORT = 9000  # port being exposed


# send command to arduino over tcp connection
def send_command(cmd, host=HOST, port=PORT, timeout=2.0):
    msg = (cmd.strip() + "\n").encode(
        "ascii"
    )  # Arduino stops reading at newline char (\n)
    try:
        with socket.create_connection((host, port), timeout=timeout) as sock:
            local_ip, local_port = sock.getsockname()
            arduino_ip, arduino_port = sock.getpeername()
            print(f"Connected {local_ip}: {local_port} -> {arduino_ip}:{arduino_port}")

            sock.sendall(msg)
            sock.shutdown(socket.SHUT_WR)  # tell server, done sending

            # Read reply
            data = sock.recv(256)
            reply = data.decode("ascii", "ignore").strip()

            print(f"Arduino Reply: {reply}; socket closing...")
            return reply
    except (socket.timeout, OSError) as e:
        print(f"Connection Failed:  {host}:{port}: {e}")
        return None
