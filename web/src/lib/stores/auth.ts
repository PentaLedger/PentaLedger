import { writable } from 'svelte/store';
import { browser } from '$app/environment';
import type { User } from '../types';

export const user = writable<User | null>(null);
export const token = writable<string | null>(null);

if (browser) {
	const storedToken = localStorage.getItem('token');
	if (storedToken) {
		token.set(storedToken);
	}
}

token.subscribe((value) => {
	if (browser) {
		if (value) {
			localStorage.setItem('token', value);
		} else {
			localStorage.removeItem('token');
		}
	}
});

