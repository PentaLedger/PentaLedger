<script lang="ts">
	import { onMount } from 'svelte';
	import { goto } from '$app/navigation';
	import { user, token } from '$lib/stores/auth';
	import { page } from '$app/stores';
	import { jwtDecode } from 'jwt-decode';

	onMount(async () => {
		const currentToken = $token;
		if (currentToken && !$user) {
			try {
				const decoded = jwtDecode<{ user_id: string; username: string }>(currentToken);
				// Optionally fetch full user data from API
				$user = {
					id: decoded.user_id,
					username: decoded.username,
					email: '' // Would need to fetch from API
				};
			} catch (e) {
				console.error('Invalid token', e);
				token.set(null);
			}
		}
	});

	$: isAuthenticated = !!$user;
	$: isAuthPage = $page.url.pathname.startsWith('/login') || $page.url.pathname.startsWith('/register');
</script>

<nav>
	<div class="container">
		<h1>PentaLedger</h1>
		{#if isAuthenticated}
			<div class="nav-links">
				<a href="/">Dashboard</a>
				<a href="/accounts">Accounts</a>
				<a href="/journal-entries">Journal Entries</a>
				<a href="/reports">Reports</a>
				<button
					onclick={() => {
						user.set(null);
						token.set(null);
						goto('/login');
					}}
				>
					Logout
				</button>
			</div>
		{/if}
	</div>
</nav>

<main>
	<slot />
</main>

<style>
	nav {
		background-color: #333;
		color: white;
		padding: 1rem;
		margin-bottom: 2rem;
	}

	nav .container {
		max-width: 1200px;
		margin: 0 auto;
		display: flex;
		justify-content: space-between;
		align-items: center;
	}

	nav h1 {
		margin: 0;
		font-size: 1.5rem;
	}

	.nav-links {
		display: flex;
		gap: 1rem;
		align-items: center;
	}

	.nav-links a {
		color: white;
		text-decoration: none;
		padding: 0.5rem 1rem;
		border-radius: 4px;
		transition: background-color 0.2s;
	}

	.nav-links a:hover {
		background-color: #555;
	}

	.nav-links button {
		background-color: #d32f2f;
		color: white;
		border: none;
		padding: 0.5rem 1rem;
		border-radius: 4px;
		cursor: pointer;
	}

	.nav-links button:hover {
		background-color: #b71c1c;
	}

	main {
		max-width: 1200px;
		margin: 0 auto;
		padding: 0 1rem;
	}
</style>
