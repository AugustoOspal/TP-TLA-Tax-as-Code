definir TOPE_MENSUAL: 50000
definir TOPE_ANUAL: 600000

regla_facturacion "Retencion Tope" {
    - monto mayor o igual a TOPE_MENSUAL
}
