definir TOPE_RETENCION: 1000000

regla_pago "Retencion Ganancias - Alquiler" {
    - es_monotributista igual a falso
    - concepto igual a "alquileres"
    - monto mayor o igual a TOPE_RETENCION
}
