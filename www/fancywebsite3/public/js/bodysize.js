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
	// Disable the submit button
	sendButton.disabled = true;
	// Get the text from the input field
	const text = textInput.value;

	// Clear input field right after the form is submitted and reset char counter
	textInput.value = '';
	charCount.textContent = '0';

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
		// Re-enable the button after response
		sendButton.disabled = false;
	})
	.catch(error => {
		alert('An error occurred: ' + error.message);
		// Re-enable the button after response
		sendButton.disabled = false;
	});
}

// Add event listener to the send button to trigger the form submission
sendButton.addEventListener('click', handleFormSubmission);
