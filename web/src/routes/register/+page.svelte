<script lang="ts">
	import { goto } from '$app/navigation';
	import { user, token } from '$lib/stores/auth';
	import { api } from '$lib/api/client';
	import type { User } from '$lib/types';

	let username = '';
	let email = '';
	let password = '';
	let confirmPassword = '';
	let error = '';
	let loading = false;

	async function handleRegister() {
		error = '';

		if (password !== confirmPassword) {
			error = 'Passwords do not match';
			return;
		}

		if (password.length < 8) {
			error = 'Password must be at least 8 characters';
			return;
		}

		loading = true;

		try {
			const response = await api.post<User>('/api/auth/register', {
				username,
				email,
				password
			});

			if (response.success && response.data) {
				// Auto-login after registration
				const loginResponse = await api.post<{ token: string; user: User }>('/api/auth/login', {
					username,
					password
				});

				if (loginResponse.success && loginResponse.data) {
					token.set(loginResponse.data.token);
					user.set(loginResponse.data.user);
					goto('/');
				}
			}
		} catch (e) {
			error = e instanceof Error ? e.message : 'Registration failed';
		} finally {
			loading = false;
		}
	}
</script>

<div class="auth-container">
	<div class="auth-card">
		<h1>Register</h1>
		{#if error}
			<div class="error">{error}</div>
		{/if}
		<form on:submit|preventDefault={handleRegister}>
			<div class="form-group">
				<label for="username">Username</label>
				<input
					type="text"
					id="username"
					bind:value={username}
					required
					disabled={loading}
				/>
			</div>
			<div class="form-group">
				<label for="email">Email</label>
				<input
					type="email"
					id="email"
					bind:value={email}
					required
					disabled={loading}
				/>
			</div>
			<div class="form-group">
				<label for="password">Password</label>
				<input
					type="password"
					id="password"
					bind:value={password}
					required
					disabled={loading}
				/>
			</div>
			<div class="form-group">
				<label for="confirmPassword">Confirm Password</label>
				<input
					type="password"
					id="confirmPassword"
					bind:value={confirmPassword}
					required
					disabled={loading}
				/>
			</div>
			<button type="submit" disabled={loading}>
				{loading ? 'Registering...' : 'Register'}
			</button>
		</form>
		<p>
			Already have an account? <a href="/login">Login here</a>
		</p>
	</div>
</div>

<style>
	.auth-container {
		display: flex;
		justify-content: center;
		align-items: center;
		min-height: 60vh;
		padding: 2rem;
	}

	.auth-card {
		background: white;
		border: 1px solid #ddd;
		border-radius: 8px;
		padding: 2rem;
		width: 100%;
		max-width: 400px;
		box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
	}

	.auth-card h1 {
		margin-top: 0;
		text-align: center;
		color: #333;
	}

	.form-group {
		margin-bottom: 1.5rem;
	}

	.form-group label {
		display: block;
		margin-bottom: 0.5rem;
		color: #555;
		font-weight: 500;
	}

	.form-group input {
		width: 100%;
		padding: 0.75rem;
		border: 1px solid #ddd;
		border-radius: 4px;
		font-size: 1rem;
		box-sizing: border-box;
	}

	.form-group input:disabled {
		background-color: #f5f5f5;
		cursor: not-allowed;
	}

	button {
		width: 100%;
		padding: 0.75rem;
		background-color: #1976d2;
		color: white;
		border: none;
		border-radius: 4px;
		font-size: 1rem;
		cursor: pointer;
		transition: background-color 0.2s;
	}

	button:hover:not(:disabled) {
		background-color: #1565c0;
	}

	button:disabled {
		background-color: #ccc;
		cursor: not-allowed;
	}

	.error {
		background-color: #ffebee;
		color: #c62828;
		padding: 0.75rem;
		border-radius: 4px;
		margin-bottom: 1rem;
	}

	p {
		text-align: center;
		margin-top: 1rem;
		color: #666;
	}

	a {
		color: #1976d2;
		text-decoration: none;
	}

	a:hover {
		text-decoration: underline;
	}
</style>

