# JSON data

Root folder may contain directories with JSON files.

Load order:
1) [Resources](#resources)
1) [Mixed](#mixed)
1) [Recipes](#recipes)
1) [Researches](#researches)
1) [Generators](#generators)
1) [Biomes](#biomes)
1) [BiomeFamilies](#biomefamilies)
1) [GlobalBiomFamily](#globalbiomfamily)

## Resources

## Mixed
#### StaticItem
```json 
{
    "class": "Item",
    "name": "CopperPlate",
    "category": "Parts",
    "image": "T_CopperPlate",
    "label_parts": [
        [
            "CopperPlate",
            "parts"
        ]
    ],
    "logic_json": {
        "Block": "CopperPlate"
    },
    "Materials": [
        "Materials/Copper"
    ],
    "Model": "Models/Plate",
    "max_count": 64,
    "tag": "Misc"
}
```

#### ItemLogic

#### StaticBlock

#### BlockLogic

## Recipes

## Researches

## Generators

## Biomes

## BiomeFamilies

## GlobalBiomFamily

```json
{
"name": "my_mod",
"version": "0.19",
"title": "My Mod",
"author": "mr. mod maker",
"evospace_version": "0.19",
"dependencies": ["vanilla >= 0.19"],
"description": "Example mod"
}
```
