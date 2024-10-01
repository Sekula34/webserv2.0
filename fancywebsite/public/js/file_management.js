document.addEventListener("DOMContentLoaded", function () {
	const uploadForm = document.getElementById('uploadForm');
	const fileInput = document.getElementById('fileInput');
	const fileList = document.getElementById('fileList');

	// Common function to format and display files
	function updateFileList(files) {
		fileList.innerHTML = ''; // Clear the current list
		files.forEach(file => {
			const fileName = file.substring(file.lastIndexOf('/') + 1);  // Extract just the file name
			const truncatedName = truncateFileName(fileName, 40);  // Truncate filenames to 40 characters
			const fileItem = document.createElement('div');
			fileItem.className = 'file-item';
			fileItem.innerHTML = `
				<a href="/uploads/${fileName}" target="_blank">${truncatedName}</a>
				<button data-file="${fileName}" class="delete-button">Delete</button>
			`;
			fileList.appendChild(fileItem);
		});
		// Call attachDeleteEventListeners after the list is updated
		attachDeleteEventListeners();
	}

	// Function to truncate file names
	function truncateFileName(fileName, maxLength) {
		const extension = fileName.slice(fileName.lastIndexOf('.')); // Get the file extension
		const baseName = fileName.slice(0, fileName.lastIndexOf('.')); // Get the base name
		if (fileName.length > maxLength) {
			return `${baseName.slice(0, maxLength - extension.length - 3)}...${extension}`; // Truncate and add "..."
		}
		return fileName;
	}

	//TODO: Debugging this function:
	function fetchFiles() {
		fetch('/uploads/')  // This path should point to your uploads folder
			.then(response => response.text())  // Get the HTML as plain text
			.then(html => {
				const parser = new DOMParser();
				const doc = parser.parseFromString(html, 'text/html');
	
				// Select the <pre> tag that contains the list of files
				const preTag = doc.querySelector('pre');
				const links = preTag.querySelectorAll('a');  // Get all <a> elements inside <pre>
	
				const files = [];
				links.forEach(link => {
					const fileName = link.getAttribute('href');
					// Ignore parent directory links and filter only files
					if (fileName !== '../' && !fileName.endsWith('/')) {
						files.push(fileName);
					}
				});
	
				// Update the file list UI
				updateFileList(files);
			})
			.catch(error => console.error('Error fetching files:', error));
	}

	function updateFileListClientSide(fileName) {
		const fileList = document.getElementById('fileList');
		
		const truncatedName = truncateFileName(fileName, 40);  // Truncate filenames to 40 characters
		const fileItem = document.createElement('div');
		fileItem.className = 'file-item';
		fileItem.innerHTML = `
			<a href="/uploads/${fileName}" target="_blank">${truncatedName}</a>
			<button data-file="${fileName}" class="delete-button">Delete</button>
		`;
		fileList.appendChild(fileItem);
		
		attachDeleteEventListeners();  // Reattach event listeners to new buttons
	}

	// Handle file upload
	uploadForm.addEventListener('submit', function (event) {
		event.preventDefault();
		const file = fileInput.files[0];
		if (!file) {
			console.error('No file selected.');
			return;
		}
		fetch(`/uploads/?${encodeURIComponent(file.name)}`, {  // Send the filename in the query string
			method: 'POST',
			body: file,  // Send the file directly as binary data
			headers: {
				'Content-Type': 'application/octet-stream'  // Specify that we're sending binary data
			}
		})
		.then(response => {
			if (response.status === 201) {  // Check if the server returned 201 Created
				console.log(`File ${file.name} uploaded successfully.`);
				updateFileListClientSide(file.name);  // Update the file list client-side
			} else {
				console.error(`Failed to upload file. Server responded with status: ${response.status}`);
			}
		})
		.catch(error => console.error('Error uploading file:', error));
		fileInput.value = '';  // Clear the file input field after upload
	});

	function removeFileFromList(fileName) {
		const fileList = document.getElementById('fileList');
		const fileItem = fileList.querySelector(`[data-file="${fileName}"]`).parentElement;
		fileList.removeChild(fileItem);  // Remove the file from the DOM
	}

	function deleteFile(fileName) {
		fetch(`/uploads/${encodeURIComponent(fileName)}`, {
			method: 'DELETE'  // Send DELETE request to the server
		})
		.then(response => {
			if (response.ok) {
				console.log(`File ${fileName} deleted successfully.`);
				removeFileFromList(fileName);  // Remove the file from the list client-side
			} else if (response.status === 404) {
				console.warn(`File ${fileName} not found (404). Removing from list.`);
				removeFileFromList(fileName);  // Remove the file from the list if not found
			} else {
				console.error(`Failed to delete ${fileName}.`);
			}
		})
		.catch(error => console.error('Error deleting file:', error));
	}

	// Attach event listeners to delete buttons
	function attachDeleteEventListeners() {
		document.querySelectorAll('.delete-button').forEach(button => {
			button.addEventListener('click', function () {
				const fileName = this.getAttribute('data-file');
				deleteFile(fileName);
			});
		});
	}

	// Initial fetch of files when the page loads
	fetchFiles();
});
