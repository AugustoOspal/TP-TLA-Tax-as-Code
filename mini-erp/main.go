package main

import (
	"encoding/json"
	"fmt"
	"html/template"
	"io/ioutil"
	"log"
	"net/http"
	"strconv"
	"strings"
)

// Data models
type Factura struct {
	CUIT             string  `json:"cuit"`
	Monto            float64 `json:"monto"`
	EsMonotributista bool    `json:"es_monotributista"`
	Concepto         string  `json:"concepto"`
}

type TaxRules struct {
	Rules     []Rule     `json:"rules"`
	Constants []Constant `json:"constants"`
}

type Rule struct {
	Name       string      `json:"name"`
	Conditions []Condition `json:"conditions"`
}

type Condition struct {
	Type  string      `json:"type"`
	Field string      `json:"field"`
	Op    int         `json:"op"`
	Value interface{} `json:"value"`
}

type Constant struct {
	Name  string  `json:"name"`
	Value float64 `json:"value"`
}

// Template data
type PageData struct {
	Processed          bool
	Matched            bool
	NoEmpadronado      bool
	RuleName           string
	MontoOriginal      string
	RetencionGanancias string
	RetencionIIBB      string
	TotalRetenido      string
	Neto               string
	Certificado        string
	CUIT               string
	CondicionFiscal    string
	AlicuotaIIBB       string
	MetodoResolucion   string
}

type Contribuyente struct {
	EsMonotributista bool
	Alicuota         float64
	CondicionFiscal  string
	Jurisdiccion     string
}

// Padrón Tributario Simulado con alícuotas y jurisdicciones (ARBA y AGIP)
var padronContribuyentes = map[string]Contribuyente{
	"30000000001": {EsMonotributista: false, Alicuota: 0.03,  CondicionFiscal: "Responsable Inscripto", Jurisdiccion: "ARBA"},
	"20000000002": {EsMonotributista: true,  Alicuota: 0.00,  CondicionFiscal: "Monotributista Exento", Jurisdiccion: "AGIP"},
	"27000000003": {EsMonotributista: true,  Alicuota: 0.015, CondicionFiscal: "Monotributista Activo", Jurisdiccion: "ARBA"},
	"33000000004": {EsMonotributista: false, Alicuota: 0.02,  CondicionFiscal: "Responsable Inscripto", Jurisdiccion: "AGIP"},
}

func main() {
	http.HandleFunc("/", handleIndex)
	http.HandleFunc("/procesar", handleProcesar)

	fmt.Println("🚀 Servidor Web Mini-ERP escuchando en http://localhost:8080")
	if err := http.ListenAndServe(":8080", nil); err != nil {
		log.Fatal("Error starting server: ", err)
	}
}

func handleIndex(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/" {
		http.NotFound(w, r)
		return
	}
	renderTemplate(w, PageData{Processed: false})
}

func handleProcesar(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Redirect(w, r, "/", http.StatusSeeOther)
		return
	}

	r.ParseForm()
	cuitRaw := r.FormValue("cuit")
	cuit := strings.ReplaceAll(strings.TrimSpace(cuitRaw), "-", "")
	monto, _ := strconv.ParseFloat(r.FormValue("monto"), 64)

	// Consulta automática al padrón por CUIT
	contribuyente, exists := padronContribuyentes[cuit]
	
	if !exists {
		data := PageData{
			Processed:     true,
			NoEmpadronado: true,
			CUIT:          cuitRaw,
		}
		renderTemplate(w, data)
		return
	}

	metodo := "Padrón Tributario Unificado (Simulado)"
	
	var alicuotaIIBB float64
	var nomJurisdiccion string
	var nomEnte string
	var jurisdiccion string

	jurisdiccion = contribuyente.Jurisdiccion
	if jurisdiccion == "AGIP" {
			nomJurisdiccion = "Ciudad Autónoma de Buenos Aires"
			nomEnte = "AGIP"
		} else {
			nomJurisdiccion = "Provincia de Buenos Aires"
			nomEnte = "ARBA"
		}

		alicuotaIIBB = contribuyente.Alicuota
		
		if alicuotaIIBB == 0 {
			metodo = fmt.Sprintf("CUIT exento o no registrado en jurisdicción %s", nomEnte)
		} else {
			metodo = fmt.Sprintf("Alícuota resuelta desde el padrón de %s", nomEnte)
		}
	factura := Factura{
		CUIT:             cuit,
		Monto:            monto,
		Concepto:         r.FormValue("concepto"),
		EsMonotributista: contribuyente.EsMonotributista,
	}

	// Read Rules dynamically
	rulesData, err := ioutil.ReadFile("../tax_rules.json")
	if err != nil {
		http.Error(w, "No se encontró tax_rules.json. Compilá las reglas primero.", 500)
		return
	}

	var taxRules TaxRules
	json.Unmarshal(rulesData, &taxRules)

	constants := make(map[string]float64)
	for _, c := range taxRules.Constants {
		constants[c.Name] = c.Value
	}

	data := PageData{
		Processed:        true,
		CUIT:             cuit,
		CondicionFiscal:  contribuyente.CondicionFiscal,
		AlicuotaIIBB:     fmt.Sprintf("%.2f%% (%s)", alicuotaIIBB*100, nomEnte),
		MetodoResolucion: metodo,
	}

	// 1. Evaluar Retención de Ganancias según el DSL Compilado (tax_rules.json)
	retencionGanancias := 0.0
	ruleName := "Ninguna"
	
	for _, rule := range taxRules.Rules {
		matched := true
		for _, cond := range rule.Conditions {
			if !evaluate(cond, factura, constants) {
				matched = false
				break
			}
		}

		if matched {
			ruleName = rule.Name
			if rule.Name == "Retencion Ganancias - Alquiler" {
				retencionGanancias = factura.Monto * 0.06 // 6% Ganancias
			}
			break
		}
	}

	// 2. Evaluar Retención de Ingresos Brutos (IIBB) según la alícuota del padrón de la jurisdicción seleccionada
	retencionIIBB := 0.0
	if alicuotaIIBB > 0 {
		retencionIIBB = factura.Monto * alicuotaIIBB
	}

	totalRetenido := retencionGanancias + retencionIIBB
	neto := factura.Monto - totalRetenido

	data.Matched = (totalRetenido > 0)
	data.RuleName = ruleName
	data.MontoOriginal = fmt.Sprintf("%.2f", factura.Monto)
	data.RetencionGanancias = fmt.Sprintf("%.2f", retencionGanancias)
	data.RetencionIIBB = fmt.Sprintf("%.2f", retencionIIBB)
	data.TotalRetenido = fmt.Sprintf("%.2f", totalRetenido)
	data.Neto = fmt.Sprintf("%.2f", neto)

	// Construir certificados diferenciados
	certText := ""
	if retencionGanancias > 0 {
		certText += fmt.Sprintf("CERTIFICADO DE RETENCION DE GANANCIAS\n---------------------------------------\nAgente de Retención: Mini ERP\nSujeto Retenido: %s (%s)\nConcepto: %s\nBase Imponible: $%.2f\nAlícuota Aplicada: 6.00%%\nMonto Retenido: $%.2f\n\n", cuit, contribuyente.CondicionFiscal, ruleName, factura.Monto, retencionGanancias)
	}
	
	if retencionIIBB > 0 {
		certText += fmt.Sprintf("CERTIFICADO DE RETENCION DE INGRESOS BRUTOS\n--------------------------------------------\nAgente de Retención: Mini ERP\nSujeto Retenido: %s (%s)\nJurisdicción: %s (%s)\nBase Imponible: $%.2f\nAlícuota de Padrón: %.2f%%\nMonto Retenido: $%.2f\n", cuit, contribuyente.CondicionFiscal, nomJurisdiccion, nomEnte, factura.Monto, alicuotaIIBB*100, retencionIIBB)
	}
	
	data.Certificado = certText
	renderTemplate(w, data)
}


func renderTemplate(w http.ResponseWriter, data PageData) {
	tmpl, err := template.ParseFiles("templates/index.html")
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	tmpl.Execute(w, data)
}

func evaluate(cond Condition, f Factura, constants map[string]float64) bool {
	if cond.Type == "relational" {
		var condValue float64
		if vMap, ok := cond.Value.(map[string]interface{}); ok {
			if vMap["type"] == "identifier" {
				condValue = constants[vMap["value"].(string)]
			} else if vMap["type"] == "string" {
				if cond.Field == "concepto" {
					return (f.Concepto == vMap["value"].(string)) == (cond.Op == 2)
				}
			}
		} else {
			switch val := cond.Value.(type) {
			case string:
				if cond.Field == "cuit" {
					return (f.CUIT == val) == (cond.Op == 2)
				}
				if cond.Field == "concepto" {
					return (f.Concepto == val) == (cond.Op == 2)
				}
			case bool:
				if cond.Field == "es_monotributista" {
					return (f.EsMonotributista == val) == (cond.Op == 2)
				}
			}
		}

		if cond.Field == "monto" {
			switch cond.Op {
			case 1:
				return f.Monto >= condValue
			case 0:
				return f.Monto <= condValue
			case 2:
				return f.Monto == condValue
			}
		}
	}
	return false
}
