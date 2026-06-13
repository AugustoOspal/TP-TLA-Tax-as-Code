definir MINIMO: 100

regla_facturacion "Logica Compleja" {
    - (monto mayor o igual a MINIMO y ingresos_brutos igual a 0) o no (es_agente_retencion igual a verdadero)
}
