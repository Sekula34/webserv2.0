import subprocess
import os
from colors import Colors
def test_executable(config_path = "configuration_files/default.conf", expected_return = 0, exec_name = "webserv"):
    # Define the root directory explicitly (two levels up from TestScript.py)
    root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../"))

    # Construct the absolute paths for the executable and the configuration file
    exec_path = os.path.join(root_dir, exec_name)
    config_file_path = os.path.join(root_dir, config_path)

    try:
        # Start the executable with provided configuration file
        process = subprocess.Popen(
            [exec_path, config_file_path],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            cwd=root_dir  # Set working directory to the root folder
        )

        # Allow 1 second for the process to complete
        my_timeout = 0.5
        try:
            stdout, stderr = process.communicate(timeout=my_timeout)
            return_code = process.returncode
            
            # Check if return code is 1
            if expected_return == 1:
                message = f"Executable returned 1. {config_path} is invalid Config."
                Colors.test_passed(message)
            else:
                message = f"Executable returned {return_code} instead of {expected_return} for {config_path}."
                Colors.test_failed(message)
            

        except subprocess.TimeoutExpired:
            # Kill the process if it exceeds 1 second
            process.kill()
            if 0 == expected_return:
                message = f"Executable is still running after {my_timeout} second. {config_path} is valid Config."
                Colors.test_passed(message)
            else:
                Colors.test_failed("Executable is still runnig but return code is not 0")

    except Exception as e:
        print(f"Error running executable: {e}")

# Usage example:
# Run `webserv` with `configuration_files/default.conf` as an argument
if __name__ == "__main__":
    test_executable("configuration_files/invalid/bulshit", 1)
    test_executable()
