<script lang="ts">
	import { onMount } from 'svelte';
	import { goto } from '$app/navigation';
	import { user } from '$lib/stores/auth';
	import { api } from '$lib/api/client';
	import type { JournalEntry } from '$lib/types';

	let entries: JournalEntry[] = [];
	let loading = true;
	let error = '';

	onMount(async () => {
		if (!$user) {
			goto('/login');
			return;
		}

		await loadEntries();
	});

	async function loadEntries() {
		loading = true;
		error = '';
		try {
			const response = await api.get<JournalEntry[]>('/api/journal-entries');
			if (response.success && response.data) {
				entries = response.data;
			}
		} catch (e) {
			error = e instanceof Error ? e.message : 'Failed to load journal entries';
		} finally {
			loading = false;
		}
	}
</script>

<div class="journal-entries-page">
	<div class="header">
		<h1>Journal Entries</h1>
		<a href="/journal-entries/new" class="btn-primary">New Entry</a>
	</div>

	{#if error}
		<div class="error">{error}</div>
	{/if}

	{#if loading}
		<p>Loading journal entries...</p>
	{:else if entries.length === 0}
		<p>No journal entries found. <a href="/journal-entries/new">Create your first entry</a></p>
	{:else}
		<div class="entries-list">
			{#each entries as entry}
				<div class="entry-card">
					<div class="entry-header">
						<h3>{entry.entry_number}</h3>
						<span class="date">{entry.date}</span>
					</div>
					<p class="description">{entry.description}</p>
					<div class="entry-lines">
						{#each entry.lines as line}
							<div class="line">
								<span class="account">{line.account_id}</span>
								<span class="amount debit">{line.debit > 0 ? `$${line.debit.toFixed(2)}` : ''}</span>
								<span class="amount credit">{line.credit > 0 ? `$${line.credit.toFixed(2)}` : ''}</span>
							</div>
						{/each}
					</div>
					<a href="/journal-entries/{entry.id}" class="view-link">View Details</a>
				</div>
			{/each}
		</div>
	{/if}
</div>

<style>
	.journal-entries-page {
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

	.entries-list {
		display: flex;
		flex-direction: column;
		gap: 1.5rem;
	}

	.entry-card {
		background: white;
		border: 1px solid #ddd;
		border-radius: 8px;
		padding: 1.5rem;
		box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
	}

	.entry-header {
		display: flex;
		justify-content: space-between;
		align-items: center;
		margin-bottom: 0.5rem;
	}

	.entry-header h3 {
		margin: 0;
		color: #333;
	}

	.date {
		color: #666;
		font-size: 0.9rem;
	}

	.description {
		color: #555;
		margin: 0.5rem 0 1rem 0;
	}

	.entry-lines {
		margin: 1rem 0;
		padding: 1rem;
		background-color: #f9f9f9;
		border-radius: 4px;
	}

	.line {
		display: grid;
	grid-template-columns: 2fr 1fr 1fr;
		gap: 1rem;
		padding: 0.5rem 0;
		border-bottom: 1px solid #eee;
	}

	.line:last-child {
		border-bottom: none;
	}

	.account {
		font-weight: 500;
	}

	.amount {
		text-align: right;
	}

	.amount.debit {
		color: #2e7d32;
	}

	.amount.credit {
		color: #c62828;
	}

	.view-link {
		display: inline-block;
		margin-top: 1rem;
		color: #1976d2;
		text-decoration: none;
	}

	.view-link:hover {
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

