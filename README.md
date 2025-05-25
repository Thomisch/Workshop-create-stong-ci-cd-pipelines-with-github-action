# Workshop: Créer des pipelines CI/CD robustes avec GitHub Actions

## Introduction

Bienvenue dans ce workshop dédié à la création de pipelines CI/CD (Continuous Integration/Continuous Deployment) avec GitHub Actions ! 

Dans le monde du développement moderne, l'automatisation des processus de build, test et déploiement est devenue essentielle. Les pipelines CI/CD permettent de :

- **Réduire les erreurs humaines** en automatisant les tâches répétitives
- **Accélérer les cycles de développement** avec des déploiements plus fréquents et fiables
- **Améliorer la qualité du code** grâce aux tests automatisés
- **Faciliter la collaboration** en équipe avec des processus standardisés
- **Détecter les problèmes plus tôt** dans le cycle de développement

GitHub Actions offre une plateforme puissante et native à GitHub pour mettre en place ces automatisations directement dans vos repositories.

## 📋 Objectifs du Workshop

À la fin de ce workshop, vous saurez :
- Comprendre les concepts fondamentaux des GitHub Actions
- Configurer des déclencheurs (triggers) appropriés
- Structurer un workflow efficace
- Gérer les secrets de manière sécurisée
- Implémenter un déploiement continu via SSH
- Intégrer des tests dans vos pipelines

## 🏗️ Structure d'un Workflow GitHub Actions

Un workflow GitHub Actions est défini dans un fichier YAML situé dans workflows. Analysons la structure de base à travers l'exemple de notre projet :

````yaml
name: Deploy to Proxmox

on:
  push:
    branches:
      - main
    paths:
      - 'Back-end/GraphQl-Service/**'
      - 'Back-end/GraphQl-Tests/**'
      - '.github/workflows/Back-end_CI.yml'

jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
      # Étapes du workflow
````

### Composants essentiels :

1. **`name`** : Nom descriptif du workflow
2. **`on`** : Définit les déclencheurs
3. **`jobs`** : Contient les tâches à exécuter
4. **`steps`** : Actions individuelles dans chaque job

## 🎯 Les Déclencheurs (Triggers)

Les déclencheurs déterminent quand votre workflow s'exécute. Voici les principaux types :

### 1. Push sur des branches spécifiques

````yaml
on:
  push:
    branches:
      - main
      - develop
````

### 2. Déclenchement conditionnel par chemins

````yaml
on:
  push:
    paths:
      - 'Back-end/GraphQl-Service/**'  # Seulement si ce dossier change
      - 'Back-end/GraphQl-Tests/**'    # Ou celui-ci
````

**Avantage** : Évite d'exécuter des workflows inutiles et économise les minutes de CI.

### 3. Autres déclencheurs utiles

````yaml
on:
  pull_request:
    branches: [main]
  
  schedule:
    - cron: '0 2 * * *'  # Tous les jours à 2h du matin
  
  workflow_dispatch:  # Déclenchement manuel
````

## 🔧 Étapes de Base d'un Pipeline

### 1. Checkout du Code

````yaml
- name: Checkout du code
  uses: actions/checkout@v3
````

**Pourquoi ?** Récupère le code source dans l'environnement d'exécution.

### 2. Configuration de l'Environnement

````yaml
- name: Setup Node.js
  uses: actions/setup-node@v3
  with:
    node-version: '18'
    cache: 'npm'
````

### 3. Installation des Dépendances

````yaml
- name: Install dependencies
  run: npm ci
````

### 4. Exécution des Tests

````yaml
- name: Run tests
  run: npm test

- name: Run integration tests
  run: npm run test:integration
````

### 5. Build de l'Application

````yaml
- name: Build application
  run: npm run build
````

## 🧪 Intégration des Tests dans votre Pipeline

### Tests Unitaires

````yaml
- name: Run unit tests
  run: |
    cd Back-end/GraphQl-Tests
    dotnet test --configuration Release --logger trx --results-directory TestResults

- name: Publish test results
  uses: dorny/test-reporter@v1
  if: success() || failure()
  with:
    name: Unit Tests
    path: Back-end/GraphQl-Tests/TestResults/*.trx
    reporter: dotnet-trx
````

### Tests d'Intégration

````yaml
- name: Start test database
  run: docker run -d --name test-db -p 5432:5432 -e POSTGRES_PASSWORD=test postgres:13

- name: Run integration tests
  run: |
    export DATABASE_URL="postgresql://postgres:test@localhost:5432/testdb"
    npm run test:integration
````

## 🔒 Gestion Sécurisée des Secrets

### Configuration dans GitHub

1. Allez dans **Settings > Secrets and variables > Actions**
2. Ajoutez vos secrets :
   - `PROXMOX_LXC_GRAPHQL_HOST`
   - `PROXMOX_GRAPHQLSERVER_SSH_PRIVATE_KEY`
   - `PROXMOX_LXC_GRAPHQL_PORT`

### Utilisation dans le Workflow

````yaml
- name: Deploy to server
  uses: appleboy/ssh-action@master
  with:
    host: ${{ secrets.PROXMOX_LXC_GRAPHQL_HOST }}
    username: cicd
    key: ${{ secrets.PROXMOX_GRAPHQLSERVER_SSH_PRIVATE_KEY }}
    port: ${{ secrets.PROXMOX_LXC_GRAPHQL_PORT }}
````

### Bonnes Pratiques pour les Secrets

- ✅ Utilisez des secrets pour toutes les informations sensibles
- ✅ Limitez les permissions des clés SSH
- ✅ Rotation régulière des secrets
- ❌ Jamais de secrets en dur dans le code

## 🚀 Déploiement Continu via SSH

### Construction et Préparation de l'Image

````yaml
- name: Build Docker image
  run: docker build -t graphql-service:latest -f ./Back-end/GraphQl-Service/Dockerfile.GraphQlServer .

- name: Save Docker image
  run: docker save graphql-service:latest -o graphql-service.tar

- name: Set permissions
  run: sudo chmod 644 graphql-service.tar
````

### Transfert via SCP

````yaml
- name: Copy image to server
  uses: appleboy/scp-action@master
  with:
    host: ${{ secrets.PROXMOX_LXC_GRAPHQL_HOST }}
    username: cicd
    key: ${{ secrets.PROXMOX_GRAPHQLSERVER_SSH_PRIVATE_KEY }}
    port: ${{ secrets.PROXMOX_LXC_GRAPHQL_PORT }}
    source: "graphql-service.tar"
    target: "/root"
````

### Déploiement via SSH

````yaml
- name: Deploy on server
  uses: appleboy/ssh-action@master
  with:
    host: ${{ secrets.PROXMOX_LXC_GRAPHQL_HOST }}
    username: cicd
    key: ${{ secrets.PROXMOX_GRAPHQLSERVER_SSH_PRIVATE_KEY }}
    port: ${{ secrets.PROXMOX_LXC_GRAPHQL_PORT }}
    script: |
      docker stop graphql-service || true
      docker rm graphql-service || true
      docker load -i /root/graphql-service.tar
      docker run -d --name graphql-service -p 5137:5137 graphql-service:latest
````

## 📊 Pipeline Complet avec Tests et Déploiement

Voici un exemple de pipeline complet intégrant tests et déploiement :

````yaml
name: Complete CI/CD Pipeline

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup .NET
        uses: actions/setup-dotnet@v3
        with:
          dotnet-version: '7.0'
      
      - name: Restore dependencies
        run: dotnet restore
      
      - name: Build
        run: dotnet build --no-restore
      
      - name: Test
        run: dotnet test --no-build --verbosity normal

  deploy:
    needs: test
    runs-on: ubuntu-latest
    if: github.ref == 'refs/heads/main'
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Build and deploy
        run: |
          docker build -t app:latest .
          # ... steps de déploiement
````

## 🎯 Exercices Pratiques

### Exercice 1 : Créer votre premier workflow
Créez un workflow qui :
- Se déclenche sur push vers `main`
- Exécute des tests unitaires
- Build l'application

### Exercice 2 : Ajouter des conditions
Modifiez le workflow pour :
- Déployer seulement depuis `main`
- Ignorer les changements dans README.md

### Exercice 3 : Gestion des secrets
Configurez un déploiement sécurisé avec :
- Clés SSH stockées en secrets
- Variables d'environnement sécurisées

## 🚨 Bonnes Pratiques et Pièges à Éviter

### ✅ Bonnes Pratiques

1. **Utilisez des versions spécifiques** des actions
   ```yaml
   uses: actions/checkout@v3  # Pas @main
   ```

2. **Optimisez vos déclencheurs**
   ```yaml
   paths-ignore:
     - '**.md'
     - 'docs/**'
   ```

3. **Mise en cache des dépendances**
   ```yaml
   - uses: actions/cache@v3
     with:
       path: ~/.npm
       key: ${{ runner.os }}-node-${{ hashFiles('**/package-lock.json') }}
   ```

### ❌ Pièges à Éviter

- Workflows trop longs (> 6h timeout)
- Secrets exposés dans les logs
- Pas de tests avant déploiement
- Déploiements sans rollback

## 🎓 Conclusion

Les GitHub Actions offrent une plateforme puissante pour automatiser vos workflows de développement. En suivant les bonnes pratiques présentées dans ce workshop, vous pourrez créer des pipelines robustes qui amélioreront significativement votre productivité et la qualité de vos déploiements.

La clé du succès réside dans la progression : commencez simple, testez, puis enrichissez progressivement vos workflows selon vos besoins.

## 📚 Ressources Supplémentaires

- [Documentation officielle GitHub Actions](https://docs.github.com/en/actions)
- [Marketplace des actions](https://github.com/marketplace)
- [Exemples de workflows](https://github.com/actions/starter-workflows)