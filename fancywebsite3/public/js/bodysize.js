// Get references to the DOM elements
const textInput = document.getElementById('textInput');
const charCount = document.getElementById('charCount');
const sendButton = document.getElementById('sendButton');

// Function to update the character count
function updateCharCount() {
	charCount.textContent = textInput.value.length;
}

// Add event listener to update the character count as the user types
textInput.addEventListener('input', updateCharCount);

// Function to handle the form submission
function handleFormSubmission() {
	// Get the text from the input field
	const text = textInput.value;

	// Send the POST request using fetch
	fetch(`/uploads/`, {  // Update this path to match your server's POST endpoint
		method: 'POST',
		headers: {
			'Content-Type': 'text/plain',  // Sending plain text data
		},
		body: text  // Send the text data directly as the request body
	})
	.then(response => {
		if (response.ok) {
			alert('Form submitted successfully!');
		} else {
			alert('Error submitting form: ' + response.statusText);
		}
	})
	.catch(error => {
		alert('An error occurred: ' + error.message);
	});
}

// Add event listener to the send button to trigger the form submission
sendButton.addEventListener('click', handleFormSubmission);
