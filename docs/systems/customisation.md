# Sistema de Personalización de Soldados y Clases (Customisation)

- **Qué hace en el juego:**
  Permite al jugador y a los perfiles de clases del juego (Rebeldes, Imperio, República, CIS) configurar el equipamiento (*Loadout*) y las estadísticas de los soldados bajo un presupuesto fijo de créditos (por defecto **100 créditos**).
  Organiza el equipamiento en **8 categorías**: 5 de equipamiento militar (`PrimaryWeapon`, `SecondaryWeapon`, `Explosive`, `Equipment`, `PowerUp`) y 3 estadísticas físicas progresivas (`Health`, `Speed`, `Agility`).
  Gestiona la validación del presupuesto, el cálculo del coste total, la sincronización con los Blueprints del motor Asura y la exposición de callbacks hacia la interfaz gráfica de carrusel (`FE_Customisation_*`).

---

## Funciones y Direcciones Relevantes

| Dirección | Nombre Propuesto | Confianza | Propósito / Notas |
|---|---|---|---|
| `0x00224878` | `Customisation_InitManager` | Alta | Inicializa el gestor de personalización y enlaza 16 callbacks con `BFF.FE_Customisation_*`. |
| `0x0022379C` | `FE_Customisation_OnInitMenu` | Alta | Callback de inicialización del menú de personalización y carga del perfil. |
| `0x00223A44` | `FE_Customisation_Credits` | Alta | Genera la cadena de visualización de créditos asignados/totales (`L"%ls %d/%d"`). |
| `0x00224148` | `FE_Customisation_ListCarouselNames` | Alta | Rellena los nombres de los ítems disponibles para una categoría en el carrusel UI. |
| `0x0022466C` | `FE_Customisation_ListCarouselCosts` | Alta | Rellena los costes en créditos de los ítems para la categoría activa. |
| `0x002304EC` | `Customisation_GetItemCost` | Alta | Resuelve el coste de un ítem: consulta `Customisation_GetStatCost` para atributos o busca la propiedad `"Cost"` (`0x002EAFCD`) en el Blueprint del arma en `COMMON.ASR`. |
| `0x002305F8` | `Customisation_GetStatCost` | Alta | Resuelve el coste de los niveles de estadísticas desde tablas estáticas en `.rodata`. |
| `0x002DBD0C` | `g_CustomisationCategoryTable` | Alta | Tabla de descriptores de categorías (8 entradas de 20 bytes: nombre, hash de categoría, ID, conteo de ítems, puntero al array de ítems). |
| `0x002DF13C` | `g_CustomisationStatCosts_Health` | Alta | Tabla de costes en créditos para Salud por nivel: `{0, 10, 20, 30}`. |
| `0x002DF14C` | `g_CustomisationStatCosts_Speed` | Alta | Tabla de costes en créditos para Velocidad por nivel: `{0, 10, 20, 30}`. |
| `0x002DF15C` | `g_CustomisationStatCosts_Agility` | Alta | Tabla de costes en créditos para Agilidad por nivel: `{0, 15, 25, 35}`. |

---

## Catálogo de Categorías e Ítems

### 1. Primary Weapon (Arma Principal) — 10 Ítems
| Índice | Blueprint / Nombre Interno | Hash Ítem | Coste (Créditos) |
|---|---|---|---|
| 0 | *None* | `0x00000000` | 0 |
| 1 | `BlasterRifle` | `0x00330D00` | 25 |
| 2 | `ArcCaster` | `0x00330D20` | 30 |
| 3 | `Carbonite_Gun` | `0x00330D40` | 15 |
| 4 | `ALL_Incinerator` | `0x00330D60` | 20 |
| 5 | `Shotgun` | `0x00330D80` | 30 |
| 6 | `Sniper Rifle` | `0x00330DA0` | 25 |
| 7 | `REP_Chaingun` | `0x00330DC0` | 40 |
| 8 | `ALL_Bow_Caster` | `0x00330DE0` | 30 |
| 9 | `Rocket_Launcher` | `0x00330E00` | 25 |

### 2. Secondary Weapon (Arma Secundaria) — 8 Ítems
| Índice | Blueprint / Nombre Interno | Hash Ítem | Coste (Créditos) |
|---|---|---|---|
| 0 | `BlasterPistol` | `0x00330E20` | 0 (Gratuito) |
| 1 | `Fusion Cutter` | `0x00330E40` | 0 (Gratuito) |
| 2 | `ALL_Tri_Shot` | `0x00330E60` | 15 |
| 3 | `Emp_Launcher` | `0x00330E80` | 25 |
| 4 | `Particle_Pistol` | `0x00330EA0` | 20 |
| 5 | `Grenade_Launcher_OnTime` | `0x00330EC0` | 20 |
| 6 | `Guided_Missile` | `0x00330EE0` | 25 |
| 7 | `Orbital Strike Gun` | `0x00330F00` | 20 |

### 3. Explosive (Explosivos) — 6 Ítems
| Índice | Blueprint / Nombre Interno | Hash Ítem | Coste (Créditos) |
|---|---|---|---|
| 0 | *None* | `0x00000000` | 0 |
| 1 | `REP_Grenade` | `0x00330F20` | 10 |
| 2 | `Det_Pack` | `0x00330F40` | 10 |
| 3 | `Proximity_Mines` | `0x00330F60` | 10 |
| 4 | `Cluster_Grenades` | `0x00330F80` | 15 |
| 5 | `Wrist_Rocket` | `0x00330FA0` | 20 |

### 4. Equipment (Equipamiento / Dispositivos) — 8 Ítems
| Índice | Blueprint / Nombre Interno | Hash Ítem | Coste (Créditos) |
|---|---|---|---|
| 0 | *None* | `0x00000000` | 0 |
| 1 | `HealthAmmo_Dispenser` | `0x00330FC0` | 10 |
| 2 | `Auto_Turret_Droid` | `0x00330FE0` | 5 |
| 3 | `Recon_Droid` | `0x00331000` | 10 |
| 4 | `Stealth_Suit` | `0x00331020` | 20 |
| 5 | `Jetpack` | `0x00331040` | 30 |
| 6 | `Jumppack` | `0x00331060` | 20 |
| 7 | `Personal_Shield` | `0x00331080` | 25 |

### 5. Power-Up (Habilidades Especiales) — 6 Ítems
| Índice | Blueprint / Nombre Interno | Hash Ítem | Coste (Créditos) |
|---|---|---|---|
| 0 | *None* | `0x00000000` | 0 |
| 1 | `Rally` | `0x003310A0` | 10 |
| 2 | `Rage` | `0x003310C0` | 15 |
| 3 | `Stamina` | `0x003310E0` | 5 |
| 4 | `Regenerate` | `0x00331100` | 15 |
| 5 | `VehicleAutoRepair` | `0x00331120` | 10 |

### 6-8. Atributos Físicos (Stats) — 4 Niveles cada uno
| Nivel (Tier) | Salud (`Health`) | Velocidad (`Speed`) | Agilidad (`Agility`) |
|---|---|---|---|
| **Tier 0** (Base) | 0 créditos | 0 créditos | 0 créditos |
| **Tier 1** | 10 créditos | 10 créditos | 15 créditos |
| **Tier 2** | 20 créditos | 20 créditos | 25 créditos |
| **Tier 3** (Máx) | 30 créditos | 30 créditos | 35 créditos |

---

## Economía de Créditos y Validaciones

- **Presupuesto Total:** El valor predeterminado es `100` créditos (`DEFAULT_TOTAL_CREDITS`).
- **Cálculo de Coste:**
  $$\text{TotalCost} = \sum_{c \in \text{Categorías}} \text{Cost}(c.\text{selectedItem})$$
- **Créditos Restantes:**
  $$\text{RemainingCredits} = \text{TotalCredits} - \text{TotalCost}$$
- **Condición de Validez:**
  $$\text{IsValidLoadout} \iff \text{TotalCost} \le \text{TotalCredits} \land (\forall c, \text{item} \in c.\text{items})$$
- **Ejemplos de Clases:**
  - **Trooper Estándar:** `BlasterRifle` (25) + `BlasterPistol` (0) + `REP_Grenade` (10) + Stats base (0) = **35 créditos** (65 restantes).
  - **Asalto Pesado:** `REP_Chaingun` (40) + `BlasterPistol` (0) + `Wrist_Rocket` (20) + `Personal_Shield` (25) + `Health Tier 1` (10) + `Stamina` (5) = **100 créditos** (0 restantes, óptimo).
  - **Inválido / Sobrepresupuesto:** `REP_Chaingun` (40) + `Jetpack` (30) + `Wrist_Rocket` (20) + `Agility Tier 2` (25) = **115 créditos** (Excede presupuesto: RECHAZADO).

---

## Integración con Blueprints (`COMMON.ASR`)

En el motor original Asura:
1. `Customisation_GetItemCost` (`0x002304EC`) examina primero si la categoría es de estadísticas (`Health`, `Speed`, `Agility`), en cuyo caso obtiene el coste de `Customisation_GetStatCost` (`0x002305F8`).
2. Para armas y dispositivos, busca el Blueprint correspondiente (`"Weapon"`) cargado desde `COMMON.ASR` y extrae la propiedad de FourCC/Hash `0x002EAFCD` (nombre `"Cost"`).
3. Si el Blueprint no define la propiedad o no está cargado en memoria, el sistema utiliza los valores por defecto tabulados en el ejecutable.

---

## Implementación C++ y Pruebas

- **Cabecera:** [include/systems/customisation.h](file:///home/jcgar2/battledecomp/include/systems/customisation.h)
- **Implementación:** [src/systems/customisation.cpp](file:///home/jcgar2/battledecomp/src/systems/customisation.cpp)
- **Pruebas Unitarias:** [tests/test_customisation.cpp](file:///home/jcgar2/battledecomp/tests/test_customisation.cpp)
- **Suite de Pruebas:** Ejecutable con `make test` junto a los tests de armas, blueprints y bots IA.
