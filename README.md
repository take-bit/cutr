# CUTR

CUTR is a URL shortening service with support for storing and caching links in PostgreSQL and Redis. The application is written in C++ using the [Drogon](https://github.com/drogonframework/drogon) framework and is fully asynchronous.

## Features

- Create short links for long URLs
- Redirect from a short link to the original URL
- Cache links in Redis to speed up redirects
- Track link usage statistics (hit counts)
- Persistent storage using PostgreSQL

## Architecture

- **Controllers**: `LinkController` handles API endpoints for creating and redirecting links
- **Services**: `LinkService` and `RedirectService` implement business logic
- **Repositories**: interact with data through the `ILinkRepository` interface and concrete implementations for PostgreSQL and Redis
- **Caching**: Redis is used for fast lookup and redirect
- **Asynchronous**: all database and cache operations are performed in coroutines

## Installation

1. Clone the repository:

```bash
git clone https://github.com/your_username/cutr.git
cd cutr
