# Import library yang diperlukan
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.metrics import confusion_matrix, classification_report
import seaborn as sns
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense
from tensorflow.keras.utils import to_categorical

# 1. Load dataset
dataset = pd.read_csv('/content/drive/MyDrive/ML ISA/akurat.csv')

# # 2. Pisahkan fitur (X) dan label (y)
X = dataset.iloc[:, :-1].values  # Semua kolom kecuali label
y = dataset.iloc[:, -1].values   # Kolom label (kategori kematangan)

# 3. One-Hot Encoding pada label
y = to_categorical(y, num_classes=3)  

# 4. Bagi data menjadi data latih dan data validasi
X_train, X_val, y_train, y_val = train_test_split(X, y, test_size=0.2, random_state=42)

# 5. Normalisasi fitur menggunakan StandardScaler
scaler = StandardScaler()
X_train = scaler.fit_transform(X_train)
X_val = scaler.transform(X_val)


# 6. Bangun model neural network
model = Sequential([
    Dense(256, input_shape=(X_train.shape[1],), activation='relu'),  # Hidden layer dengan 256 neuron
    Dense(3, activation='softmax')  # Output layer dengan 3 neuron (3 kategori)
])

# 7. Kompilasi model
model.compile(optimizer='adam', loss='categorical_crossentropy', metrics=['accuracy'])

# 8. Latih model
history = model.fit(X_train, y_train, epochs=100, batch_size=32, validation_data=(X_val, y_val))

# 9. Evaluasi model
loss, accuracy = model.evaluate(X_val, y_val)
print(f'Akurasi model pada data validasi: {accuracy * 100:.2f}%')

# 10. Simpan model jika diperlukan
model.save('model_akurat.h5')

# Mendapatkan prediksi model
y_pred = model.predict(X_val)
y_pred_classes = np.argmax(y_pred, axis=1)  # Konversi one-hot encoding ke label kelas
y_true = np.argmax(y_val, axis=1)          # Konversi one-hot encoding ke label kelas

# Membuat confusion matrix
cm = confusion_matrix(y_pred_classes, y_true)

# Visualisasi confusion matrix
plt.figure(figsize=(6, 4))
sns.heatmap(cm, annot=True, fmt='d', cmap='Blues',
            xticklabels=['Mentah', 'Matang', 'terlalu matang'],
            yticklabels=['Mentah', 'Matang', 'terlalu matang'])
plt.title('Confusion Matrix')
plt.xlabel('Label Sebenarnya')
plt.ylabel('Prediksi Model')
plt.show()

# Menampilkan classification report
print("\nClassification Report:")
print(classification_report(y_true, y_pred_classes, target_names=['Mentah', 'Matang', 'terlalu matang']))

