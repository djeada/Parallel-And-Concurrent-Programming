import aiohttp
import asyncio


async def fetch(url, session):
    async with session.get(url) as response:
        content = await response.text()
        print(f"Fetched content from {url} with length {len(content)}")


async def fetch_all(urls):
    async with aiohttp.ClientSession() as session:
        tasks = [fetch(url, session) for url in urls]
        await asyncio.gather(*tasks)


async def main():
    urls = [
        "https://httpbin.org/get",
        "https://api.github.com",
        "https://www.python.org",
        "https://www.example.com",
    ]

    await fetch_all(urls)


if __name__ == "__main__":
    asyncio.run(main())
