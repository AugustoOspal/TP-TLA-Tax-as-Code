definir TOPE_RETENCION: 1000000
definir FACTURACION_MINIMA: 500000 + 200000
definir SCORE_CRITICO: 50 * 2
definir TASA_GENERAL: 6%

regla_pago "Retencion Ganancias - Alquiler" {
    - es_monotributista igual a falso
    - concepto igual a "alquileres"
    - monto mayor o igual a TOPE_RETENCION
}

regla_facturacion "Alerta Control Fiscal" {
    - es_monotributista igual a verdadero
    - monto mayor o igual a FACTURACION_MINIMA
    - score mayor o igual a SCORE_CRITICO
}

regla_exencion "Exencion PyME" {
    - es_pyme igual a verdadero
    - monto menor o igual a TOPE_RETENCION * 2.5
}
