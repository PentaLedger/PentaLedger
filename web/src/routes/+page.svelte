<script lang="ts">
	import { onMount } from 'svelte';
	import { goto } from '$app/navigation';
	import { user } from '$lib/stores/auth';

	onMount(() => {
		if (!$user) {
			goto('/login');
		}
	});
</script>

{#if $user}
	<div class="dashboard">
		<h1>Dashboard</h1>
		<p>Welcome to PentaLedger, {$user.username}!</p>
		<div class="cards">
			<div class="card">
				<h2>Accounts</h2>
				<p>Manage your chart of accounts</p>
				<a href="/accounts">View Accounts</a>
			</div>
			<div class="card">
				<h2>Journal Entries</h2>
				<p>Create and view journal entries</p>
				<a href="/journal-entries">View Entries</a>
			</div>
			<div class="card">
				<h2>Reports</h2>
				<p>View financial reports</p>
				<a href="/reports">View Reports</a>
			</div>
		</div>
	</div>
{/if}

<style>
	.dashboard {
		padding: 2rem 0;
	}

	.dashboard h1 {
		margin-bottom: 1rem;
	}

	.cards {
		display: grid;
		grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
		gap: 1.5rem;
		margin-top: 2rem;
	}

	.card {
		background: white;
		border: 1px solid #ddd;
		border-radius: 8px;
		padding: 1.5rem;
		box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
	}

	.card h2 {
		margin-top: 0;
		color: #333;
	}

	.card p {
		color: #666;
		margin: 0.5rem 0 1rem 0;
	}

	.card a {
		display: inline-block;
		background-color: #1976d2;
		color: white;
		padding: 0.5rem 1rem;
		border-radius: 4px;
		text-decoration: none;
		transition: background-color 0.2s;
	}

	.card a:hover {
		background-color: #1565c0;
	}
</style>
