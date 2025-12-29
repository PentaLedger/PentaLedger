<script lang="ts">
	import { onMount } from 'svelte';
	import { goto } from '$app/navigation';
	import { user } from '$lib/stores/auth';
	import { api } from '$lib/api/client';
	import type { Account } from '$lib/types';

	let accounts: Account[] = [];
	let loading = true;
	let error = '';

	onMount(async () => {
		if (!$user) {
			goto('/login');
			return;
		}

		await loadAccounts();
	});

	async function loadAccounts() {
		loading = true;
		error = '';
		try {
			const response = await api.get<Account[]>('/api/accounts');
			if (response.success && response.data) {
				accounts = response.data;
			}
		} catch (e) {
			error = e instanceof Error ? e.message : 'Failed to load accounts';
		} finally {
			loading = false;
		}
	}
</script>

<div class="accounts-page">
	<div class="header">
		<h1>Chart of Accounts</h1>
		<a href="/accounts/new" class="btn-primary">Add Account</a>
	</div>

	{#if error}
		<div class="error">{error}</div>
	{/if}

	{#if loading}
		<p>Loading accounts...</p>
	{:else if accounts.length === 0}
		<p>No accounts found. <a href="/accounts/new">Create your first account</a></p>
	{:else}
		<table>
			<thead>
				<tr>
					<th>Code</th>
					<th>Name</th>
					<th>Type</th>
					<th>Active</th>
					<th>Actions</th>
				</tr>
			</thead>
			<tbody>
				{#each accounts as account}
					<tr>
						<td>{account.code}</td>
						<td>{account.name}</td>
						<td>{account.type}</td>
						<td>{account.is_active ? 'Yes' : 'No'}</td>
						<td>
							<a href="/accounts/{account.id}">View</a>
						</td>
					</tr>
				{/each}
			</tbody>
		</table>
	{/if}
</div>

<style>
	.accounts-page {
		padding: 2rem 0;
	}

	.header {
		display: flex;
		justify-content: space-between;
		align-items: center;
		margin-bottom: 2rem;
	}

	.btn-primary {
		background-color: #1976d2;
		color: white;
		padding: 0.75rem 1.5rem;
		border-radius: 4px;
		text-decoration: none;
		transition: background-color 0.2s;
	}

	.btn-primary:hover {
		background-color: #1565c0;
	}

	table {
		width: 100%;
		border-collapse: collapse;
		background: white;
		border: 1px solid #ddd;
		border-radius: 8px;
		overflow: hidden;
	}

	thead {
		background-color: #f5f5f5;
	}

	th,
	td {
		padding: 1rem;
		text-align: left;
		border-bottom: 1px solid #ddd;
	}

	th {
		font-weight: 600;
		color: #333;
	}

	tbody tr:hover {
		background-color: #f9f9f9;
	}

	a {
		color: #1976d2;
		text-decoration: none;
	}

	a:hover {
		text-decoration: underline;
	}

	.error {
		background-color: #ffebee;
		color: #c62828;
		padding: 1rem;
		border-radius: 4px;
		margin-bottom: 1rem;
	}
</style>

