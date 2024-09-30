document.addEventListener("DOMContentLoaded", function () {
	const uploadForm = document.getElementById('uploadForm');
	const fileInput = document.getElementById('fileInput');
	const fileList = document.getElementById('fileList');

	// Fetch and display the list of files
	// function fetchFiles() {
	// 	fetch('/uploads/')
	// 		.then(response => response.json())
	// 		.then(files => {
	// 			updateFileList(files); // Use the common function to format and display files
	// 		})
	// 		.catch(error => console.error('Error fetching files:', error));
	// }

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

	// 	// Add event listeners to delete buttons
	// 	attachDeleteEventListeners();
	// }

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

	// function fetchFiles() {
	// 	fetch('/uploads/')  // Fetch the autoindex page
	// 		.then(response => response.text())  // Get the HTML as plain text
	// 		.then(html => {
	// 			console.log("Received HTML:", html);  // Log the received HTML
	
	// 			const parser = new DOMParser();
	// 			const doc = parser.parseFromString(html, 'text/html');
	
	// 			// Log the parsed document to ensure it's processed correctly
	// 			console.log("Parsed Document:", doc);
	
	// 			// Select the <pre> tag that contains the list of files
	// 			const preTag = doc.querySelector('pre');
	// 			if (!preTag) {
	// 				console.error("No <pre> tag found in the HTML");
	// 				return;
	// 			}
	
	// 			const links = preTag.querySelectorAll('a');  // Get all <a> elements inside <pre>
	
	// 			const files = [];
	// 			links.forEach(link => {
	// 				const fileName = link.getAttribute('href');
	// 				// Ignore parent directory links and filter only files
	// 				if (fileName !== '../' && !fileName.endsWith('/')) {
	// 					files.push(fileName);
	// 				}
	// 			});
	
	// 			console.log("Extracted files:", files);  // Log the extracted file names
	
	// 			// Update the file list UI
	// 			updateFileList(files);
	// 		})
	// 		.catch(error => console.error('Error fetching files:', error));
	// }

	// TODO: Debugging this function:
	// Handle file upload
	// uploadForm.addEventListener('submit', function (event) {
	// 	event.preventDefault();
	// 	const formData = new FormData();
	// 	formData.append('file', fileInput.files[0]);

	// 	fetch(`/fancywebsite/pages/uploads/${fileInput.files[0].name}`, {
	// 		method: 'POST',
	// 		body: fileInput.files[0], // Sending the file as binary
	// 		headers: {
	// 			'Content-Type': 'application/octet-stream' // Mimicking binary upload
	// 		}
	// 	})
	// 		.then(response => response.json())
	// 		.then(files => {
	// 			// alert('File uploaded successfully!');
	// 			fileInput.value = ''; // Clear the file input field after upload
	// 			updateFileList(files); // Use the response to update the file list
	// 		})
	// 		.catch(error => console.error('Error uploading file:', error));
	// });

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
		.then(() => {
			console.log(`File ${file.name} uploaded successfully.`);// alert('File uploaded successfully!');
			fileInput.value = ''; // Clear the file input field after upload
			updateFileListClientSide(file.name);  // Update the file list client-side
		})
		.catch(error => console.error('Error uploading file:', error));
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

	// TODO: Debugging this function:
	// Handle file deletion using GET request
	function deleteFile(fileName) {
		fetch(`/uploads/${encodeURIComponent(fileName)}`, {
			method: 'DELETE'  // Send DELETE request to the server
		})
		.then(response => {
			if (response.ok) {
				console.log(`File ${fileName} deleted successfully.`);
				fetchFiles();  // Refresh the file list after deletion
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
