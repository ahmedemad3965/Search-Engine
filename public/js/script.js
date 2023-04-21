// Add event listener to search form
const form = document.querySelector('form');
form.addEventListener('submit', function(event) {
	event.preventDefault();
	const searchInput = form.querySelector('input[type="text"]');
	const searchQuery = searchInput.value;

    // goto /search?q=SEARCH_QUERY
    window.location.href = '/search?q=' + searchQuery;
    
});
