# GitHub Pages dev environment

This setup allows debugging the website in a local Docker environment, before publishing online.

1. Run the docker container:

    ```bash
    docker compose up --build
    ```

2. Open http://0.0.0.0:4000 in your browser
3. Enjoy

The Jekyll server updates the page automatically on changes. You might have to restart the container when adding new pages though or changing the `_config.yml`