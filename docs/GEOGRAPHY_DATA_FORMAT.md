# Formato dos dados geográficos

O cenário é carregado de CSVs UTF-8. IDs são inteiros positivos e estáveis dentro de cada tipo de entidade. Nomes são rótulos; referências usam IDs, nunca nomes.

## Hierarquia

A hierarquia é:

`continente -> região -> sub-região -> área -> província`

### geography.csv

Cabeçalho obrigatório:

`type,id,name,parent_id`

Tipos aceitos:

| type | parent_id aponta para | Regra |
|---|---|---|
| continent | nenhum (0) | ID único |
| region | continente | ID e nome devem existir em `regions.csv` |
| subregion | região | ID único |
| area | sub-região | ID único |

Quando `geography.csv` está presente, todas as regiões precisam estar associadas a um continente, e o arquivo precisa conter pelo menos um continente, sub-região e área.

### province_areas.csv

Cabeçalho: `province_id,area_id`

Cada província deve aparecer exatamente uma vez. A área deve existir e sua sub-região precisa pertencer à mesma região declarada na linha da província em `provinces.csv`.

## Conexões

### adjacencies.csv

Formato legado: `province_a,province_b`

Cada linha representa uma ligação terrestre bidirecional. O formato é mantido para compatibilidade com cenários anteriores.

### connections.csv

Cabeçalho: `province_a,province_b,type,one_way`

Tipos aceitos:

- `land`: ligação terrestre.
- `sea`: ligação marítima.
- `strait`: estreito.
- `river`: ligação fluvial.
- `impassable`: fronteira física intransponível.

`one_way` aceita `0` (bidirecional) ou `1` (A -> B). Para uma ligação bidirecional, a ordem dos IDs não altera a identidade da conexão. Um par pode ter tipos distintos, por exemplo uma fronteira terrestre e um estreito representado separadamente.

Uma ligação intransponível conta como vizinhança física em `areAdjacent`, mas não como rota em `hasPassableConnection`. Ligações direcionais só permitem passagem no sentido cadastrado.

## Compatibilidade e validação

- Os arquivos de hierarquia são opcionais para cenários legados.
- Se `geography.csv` existir, `province_areas.csv` passa a ser obrigatório e deve mapear todas as províncias.
- `connections.csv` é opcional; as linhas de `adjacencies.csv` continuam sendo carregadas como terra bidirecional.
- IDs repetidos, referências inexistentes, tipos desconhecidos, campos inválidos e conexões duplicadas do mesmo tipo são rejeitados.
- A carga é transacional: se qualquer arquivo falhar, o mundo anteriormente carregado permanece intacto.

Os dados atuais são um cenário fictício pequeno para teste de estrutura, não uma base cartográfica histórica.
