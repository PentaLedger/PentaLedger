# Build Stage
FROM node:alpine AS builder
WORKDIR /app
COPY package.json ./
RUN npm install
COPY . .
RUN npm install -D @sveltejs/adapter-static
RUN npm run build 

FROM nginx:alpine
COPY --from=builder /app/build /usr/share/nginx/html
COPY conf/nginx.conf /etc/nginx/conf.d/default.conf

EXPOSE 8080

CMD ["nginx", "-g", "daemon off;"]