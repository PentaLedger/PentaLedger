export interface User {
	id: string;
	username: string;
	email: string;
}

export interface Account {
	id: string;
	code: string;
	name: string;
	type: 'ASSET' | 'LIABILITY' | 'EQUITY' | 'REVENUE' | 'EXPENSE';
	parent_id?: string;
	description?: string;
	is_active: boolean;
}

export interface JournalEntryLine {
	id: string;
	account_id: string;
	debit: number;
	credit: number;
	description?: string;
	line_order: number;
}

export interface JournalEntry {
	id: string;
	entry_number: string;
	date: string;
	description: string;
	created_at: string;
	lines: JournalEntryLine[];
}

export interface ApiResponse<T> {
	success: boolean;
	data?: T;
	error?: string;
}

