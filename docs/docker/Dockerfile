# This file is based on
# https://github.com/BretFisher/jekyll-serve/blob/main/Dockerfile

# Start from a light-weight ruby image
# Version as used by GitHub Pages, see https://pages.github.com/versions
FROM ruby:3.3.4-alpine

# Install gcc, make and git
RUN apk add build-base git

# Install jekyll
RUN gem install jekyll bundler

# Make directory for site
WORKDIR /site

# The Jekyll server runs on port 4000 by default
EXPOSE 4000

# Install the required gems for the site
COPY Gemfile /site/
RUN bundle install

# Override the GitHub Pages repo name
ENV PAGES_REPO_NWO="KIT-MRT/arbitration_graphs"

# Run jekyll-serve for dynamic development
CMD [ "bundle", "exec", "jekyll", "serve", "--livereload", "--force_polling", "-H", "0.0.0.0", "-P", "4000"]