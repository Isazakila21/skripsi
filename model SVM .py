# 1. Import Library

import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split, GridSearchCV
from sklearn.preprocessing import StandardScaler
from sklearn.svm import SVC
from sklearn.pipeline import Pipeline
from sklearn.metrics import classification_report, accuracy_score
#from micromlgen import port


# 2. Load Dataset
# Pastikan CSV kamu sudah berisi kolom R, G, B, dan Label
data = pd.read_csv('/content/drive/MyDrive/ML ISA/akurat.csv')

# Pisahkan fitur dan label
X = data[['R', 'G', 'B']].values
y = data['Label'].values
y = data.iloc[:, -1].astype(int).values

# 3. Bagi data Train/Test
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42, stratify=y)

# 4. Buat pipeline SVM + Scaler

pipeline = Pipeline([
    ('scaler', StandardScaler()),
    ('svm', SVC(kernel='rbf'))
])


# 5. Parameter Grid Search
param_grid = {
    'svm__C': [0.1, 1, 10, 100],
    'svm__gamma': [0.001, 0.01, 0.1, 1]
}


# 6. Grid Search + Cross Validation
grid_search = GridSearchCV(
    estimator=pipeline,
    param_grid=param_grid,
    scoring='accuracy',
    cv=5,          # 5-fold cross validation
    verbose=2
)

grid_search.fit(X_train, y_train)

# 7. Hasil terbaik
print("Best Parameters:", grid_search.best_params_)
print("Best Cross-Validation Accuracy:", grid_search.best_score_)

# Model terbaik
best_model = grid_search.best_estimator_


# 8. Evaluasi di data test
y_pred = best_model.predict(X_test)
print("Test Accuracy:", accuracy_score(y_test, y_pred))
print(classification_report(y_test, y_pred, target_names=['Mentah', 'Matang', 'Terlalu Matang']))


# 9. Export ke Arduino (C code)
# Ambil hanya model SVM (tanpa scaler, scaler harus diterapkan manual di Arduino)
svm_model = best_model.named_steps['svm']

# Pastikan gamma diubah ke float sebelum export
svm_model.gamma = float(svm_model.gamma)

# Export
# c_code = port(svm_model, class_names=['Mentah', 'Matang', 'Terlalu Matang'])
# print(c_code)

with open("model_svm.h", "w") as f:
    f.write(c_code)

print("Model berhasil disimpan ke model_svm.h")
