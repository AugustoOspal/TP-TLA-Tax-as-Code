# Dockerfile para el Mini-ERP en Go
FROM golang:1.22-alpine

WORKDIR /app

# Copiar archivos necesarios para compilar de forma independiente
COPY tax_rules.json ./
COPY mini-erp/ ./mini-erp/

WORKDIR /app/mini-erp
RUN go build -o main .

EXPOSE 8080

CMD ["./main"]
