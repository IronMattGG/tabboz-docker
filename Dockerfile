# Rebuild the game with the pinned SDK and the locally patched Novantotto.
FROM emscripten/emsdk:3.1.56@sha256:f9479effb0d095fea1c8501b57c18d13876a5e3bfebcb71f0da68a0fe49c935f AS build
ENV SOURCE_DATE_EPOCH=1720027464
WORKDIR /src
COPY Makefile *.c *.h ./
COPY vendor/novantotto/ vendor/novantotto/
COPY tests/novantotto-*.c tests/
RUN make NODE=/emsdk/node/16.20.0_64bit/bin/node test build

# Nginx 1.30.6 (official stable Alpine image), pinned for reproducible builds.
FROM nginx:stable-alpine@sha256:47928bf67eb62a1863cb08d9ecb52d3e704304ba614eaee17884204677702951

COPY nginx.conf /etc/nginx/conf.d/default.conf
COPY index.html /usr/share/nginx/html/
COPY --from=build /src/zarrosim.js /src/zarrosim.wasm /usr/share/nginx/html/
COPY resources/ /usr/share/nginx/html/resources/
COPY LICENSE THIRD_PARTY.md /usr/share/nginx/html/licenses/
COPY licenses/ /usr/share/nginx/html/licenses/

# Resource generators and directory listings are not used by the web runtime.
RUN find /usr/share/nginx/html/resources -type f \( -name '*.rc' -o -name '*.post' -o -name 'index.html' \) -delete \
    && rm -rf /usr/share/nginx/html/resources/dialogs/html \
    && rm -f /usr/share/nginx/html/50x.html

EXPOSE 80
