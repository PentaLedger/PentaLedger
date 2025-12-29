<script lang="ts">
	import { onMount } from 'svelte';
	import { goto } from '$app/navigation';
	import { user } from '$lib/stores/auth';
	import { api } from '$lib/api/client';

	let balanceSheet: any = null;
	let incomeStatement: any = null;
	let loading = true;
	let error = '';

	onMount(async () => {
		if (!$user) {
			goto('/login');
			return;
		}

		await loadReports();
	});

	async function loadReports() {
		loading = true;
		error = '';
		try {
			const bsResponse = await api.get('/api/reports/balance-sheet');
			const isResponse = await api.get('/api/reports/income-statement');

			if (bsResponse.success && bsResponse.data) {
				balanceSheet = bsResponse.data;
			}
			if (isResponse.success && isResponse.data) {
				incomeStatement = isResponse.data;
			}
		} catch (e) {
			error = e instanceof Error ? e.message : 'Failed to load reports';
		} finally {
			loading = false;
		}
	}
</script>

<div class="reports-page">
	<h1>Financial Reports</h1>

	{#if error}
		<div class="error">{error}</div>
	{/if}

	{#if loading}
		<p>Loading reports...</p>
	{:else}
		<div class="reports-grid">
			{#if balanceSheet}
				<div class="report-card">
					<h2>Balance Sheet</h2>
					{#each balanceSheet.sections as section}
						<div class="section">
							<h3>{section.type}</h3>
							{#each section.accounts as account}
								<div class="account-line">
									<span class="account-name">{account.name}</span>
									<span class="account-balance">${account.balance.toFixed(2)}</span>
								</div>
							{/each}
							<div class="section-total">
								<strong>Total {section.type}: ${section.total.toFixed(2)}</strong>
							</div>
						</div>
					{/each}
					<div class="report-summary">
						<p>Total Assets: ${balanceSheet.total_assets.toFixed(2)}</p>
						<p>Total Liabilities: ${balanceSheet.total_liabilities.toFixed(2)}</p>
						<p>Total Equity: ${balanceSheet.total_equity.toFixed(2)}</p>
					</div>
				</div>
			{/if}

			{#if incomeStatement}
				<div class="report-card">
					<h2>Income Statement</h2>
					{#each incomeStatement.sections as section}
						<div class="section">
							<h3>{section.type}</h3>
							{#each section.accounts as account}
								<div class="account-line">
									<span class="account-name">{account.name}</span>
									<span class="account-balance">${account.balance.toFixed(2)}</span>
								</div>
							{/each}
							<div class="section-total">
								<strong>Total {section.type}: ${section.total.toFixed(2)}</strong>
							</div>
						</div>
					{/each}
					<div class="report-summary">
						<p>Total Revenue: ${incomeStatement.total_revenue.toFixed(2)}</p>
						<p>Total Expenses: ${incomeStatement.total_expenses.toFixed(2)}</p>
						<p class="net-income">
							<strong>Net Income: ${incomeStatement.net_income.toFixed(2)}</strong>
						</p>
					</div>
				</div>
			{/if}
		</div>
	{/if}
</div>

<style>
	.reports-page {
		padding: 2rem 0;
	}

	.reports-grid {
		display: grid;
		grid-template-columns: repeat(auto-fit, minmax(400px, 1fr));
		gap: 2rem;
		margin-top: 2rem;
	}

	.report-card {
		background: white;
		border: 1px solid #ddd;
		border-radius: 8px;
		padding: 2rem;
		box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
	}

	.report-card h2 {
		margin-top: 0;
		color: #333;
		border-bottom: 2px solid #1976d2;
		padding-bottom: 0.5rem;
	}

	.section {
		margin: 1.5rem 0;
	}

	.section h3 {
		color: #555;
		font-size: 1.1rem;
		margin-bottom: 0.5rem;
	}

	.account-line {
		display: flex;
		justify-content: space-between;
		padding: 0.5rem 0;
		border-bottom: 1px solid #eee;
	}

	.account-name {
		color: #666;
	}

	.account-balance {
		font-weight: 500;
		color: #333;
	}

	.section-total {
		margin-top: 0.5rem;
		padding-top: 0.5rem;
		border-top: 2px solid #ddd;
		text-align: right;
	}

	.report-summary {
		margin-top: 2rem;
		padding-top: 1rem;
		border-top: 2px solid #1976d2;
	}

	.report-summary p {
		margin: 0.5rem 0;
		text-align: right;
	}

	.net-income {
		font-size: 1.2rem;
		margin-top: 1rem !important;
		padding-top: 1rem;
		border-top: 2px solid #ddd;
	}

	.error {
		background-color: #ffebee;
		color: #c62828;
		padding: 1rem;
		border-radius: 4px;
		margin-bottom: 1rem;
	}
</style>

