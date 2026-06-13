package main

import (
	"encoding/json"
	"fmt"
	"html/template"
	"io/ioutil"
	"log"
	"net/http"
	"strconv"
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
	Processed     bool
	Matched       bool
	RuleName      string
	MontoOriginal string
	Retencion     string
	Neto          string
	Certificado   string
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
	monto, _ := strconv.ParseFloat(r.FormValue("monto"), 64)
	esMono := r.FormValue("monotributista") == "on"

	factura := Factura{
		CUIT:             r.FormValue("cuit"),
		Monto:            monto,
		Concepto:         r.FormValue("concepto"),
		EsMonotributista: esMono,
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

	data := PageData{Processed: true, Matched: false}

	// Evaluate
	for _, rule := range taxRules.Rules {
		matched := true
		for _, cond := range rule.Conditions {
			if !evaluate(cond, factura, constants) {
				matched = false
				break
			}
		}

		if matched {
			data.Matched = true
			data.RuleName = rule.Name

			if rule.Name == "Retencion Ganancias - Alquiler" {
				retencion := factura.Monto * 0.06
				neto := factura.Monto - retencion

				data.MontoOriginal = fmt.Sprintf("%.2f", factura.Monto)
				data.Retencion = fmt.Sprintf("%.2f", retencion)
				data.Neto = fmt.Sprintf("%.2f", neto)
				
				cert := fmt.Sprintf("CERTIFICADO DE RETENCION\n------------------------\nAgente: Mini ERP\nSujeto: %s\nBase Imponible: $%.2f\nRetenido: $%.2f", factura.CUIT, factura.Monto, retencion)
				data.Certificado = cert
			}
			break
		}
	}

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
